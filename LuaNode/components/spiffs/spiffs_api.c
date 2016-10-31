//#include "user_config.h"
#include "spiffs.h"
#include "rom/spi_flash.h"
#include "c_stdlib.h"
#include "c_string.h"

#define SPIFFS_START_ADDRESS	0x100000
#define SPIFFS_END_ADDRESS		0x1FB000
#define SPIFFS_BLOCK_SIZE		8192
#define SPIFFS_PAGE_SIZE		256

spiffs _fs;

u32_t _start = SPIFFS_START_ADDRESS;
u32_t _size = (uint32_t) (SPIFFS_END_ADDRESS) - (uint32_t) (SPIFFS_START_ADDRESS);
u32_t _blockSize = SPIFFS_BLOCK_SIZE;
u32_t _pageSize = SPIFFS_PAGE_SIZE;

static u8_t *_workBuf;
static u8_t *_fdsBuf;
static u8_t *_cacheBuf;


#ifndef GET_MIN
#define GET_MIN(x, y)	((x<y) ? (x):(y))
#endif

bool flashRead(uint32_t offset, uint32_t *data, size_t size) {
    int rc = SPIRead(offset, (uint32_t*) data, size);
    return rc == 0;
}

bool flashWrite(uint32_t offset, uint32_t *data, size_t size) {
    int rc = SPIWrite(offset, (uint32_t*) data, size);
    return rc == 0;
}

bool flashEraseSector(uint32_t sector) {
    int rc = SPIEraseSector(sector);
    return rc == 0;
}

s32_t spiffs_hal_read(u32_t addr, u32_t size, u8_t *dst) {
    uint32_t result = SPIFFS_OK;
    uint32_t alignedBegin = (addr + 3) & (~3);
    uint32_t alignedEnd = (addr + size) & (~3);
    if (alignedEnd < alignedBegin) {
        alignedEnd = alignedBegin;
    }

    if (addr < alignedBegin) {
        uint32_t nb = alignedBegin - addr;
        uint32_t tmp;
        if (!flashRead(alignedBegin - 4, &tmp, 4)) {
            //NODE_DBG("_spif_read(%d) addr=%x size=%x ab=%x ae=%x\r\n",
            //    __LINE__, addr, size, alignedBegin, alignedEnd);
            return SPIFFS_ERR_INTERNAL;
        }
        memcpy(dst, &tmp + 4 - nb, nb);
    }

    if (alignedEnd != alignedBegin) {
        if (!flashRead(alignedBegin, (uint32_t*) (dst + alignedBegin - addr),
                alignedEnd - alignedBegin)) {
            //NODE_DBG("_spif_read(%d) addr=%x size=%x ab=%x ae=%x\r\n",
            //    __LINE__, addr, size, alignedBegin, alignedEnd);
            return SPIFFS_ERR_INTERNAL;
        }
    }

    if (addr + size > alignedEnd) {
        uint32_t nb = addr + size - alignedEnd;
        uint32_t tmp;
        if (!flashRead(alignedEnd, &tmp, 4)) {
           // NODE_DBG("_spif_read(%d) addr=%x size=%x ab=%x ae=%x\r\n",
            //    __LINE__, addr, size, alignedBegin, alignedEnd);
            return SPIFFS_ERR_INTERNAL;
        }

        memcpy(dst + size - nb, &tmp, nb);
    }

    return result;
}

/*
 Like spi_flash_read, spi_flash_write has a requirement for flash address to be
 aligned. However it also requires RAM address to be aligned as it reads data
 in 32-bit words. Flash address (mis-)alignment is handled much the same way
 as for reads, but for RAM alignment we have to copy data into a temporary
 buffer. The size of this buffer is a tradeoff between number of writes required
 and amount of stack required. This is chosen to be 512 bytes here, but might
 be adjusted in the future if there are good reasons to do so.
*/

static const int UNALIGNED_WRITE_BUFFER_SIZE = 512;

s32_t spiffs_hal_write(u32_t addr, u32_t size, u8_t *src) {
    uint32_t alignedBegin = (addr + 3) & (~3);
    uint32_t alignedEnd = (addr + size) & (~3);
    if (alignedEnd < alignedBegin) {
        alignedEnd = alignedBegin;
    }

    if (addr < alignedBegin) {
        uint32_t ofs = alignedBegin - addr;
        uint32_t nb = (size < ofs) ? size : ofs;
        uint8_t tmp[4] __attribute__((aligned(4))) = {0xff, 0xff, 0xff, 0xff};
        memcpy(tmp + 4 - ofs, src, nb);
        if (!flashWrite(alignedBegin - 4, (uint32_t*) tmp, 4)) {
            //NODE_DBG("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
            //    __LINE__, addr, size, alignedBegin, alignedEnd);
            return SPIFFS_ERR_INTERNAL;
        }
    }

    if (alignedEnd != alignedBegin) {
        uint32_t* srcLeftover = (uint32_t*) (src + alignedBegin - addr);
        uint32_t srcAlign = ((uint32_t) srcLeftover) & 3;
        if (!srcAlign) {
            if (!flashWrite(alignedBegin, (uint32_t*) srcLeftover,
                    alignedEnd - alignedBegin)) {
                //NODE_DBG("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
                //    __LINE__, addr, size, alignedBegin, alignedEnd);
                return SPIFFS_ERR_INTERNAL;
            }
        }
        else {
            uint8_t buf[UNALIGNED_WRITE_BUFFER_SIZE];
            for (uint32_t sizeLeft = alignedEnd - alignedBegin; sizeLeft; ) {
                size_t willCopy = GET_MIN(sizeLeft, sizeof(buf));
                memcpy(buf, srcLeftover, willCopy);

                if (!flashWrite(alignedBegin, (uint32_t*) buf, willCopy)) {
                    //NODE_DBG("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
                    //    __LINE__, addr, size, alignedBegin, alignedEnd);
                    return SPIFFS_ERR_INTERNAL;
                }

                sizeLeft -= willCopy;
                srcLeftover += willCopy;
                alignedBegin += willCopy;
            }
        }
    }

    if (addr + size > alignedEnd) {
        uint32_t nb = addr + size - alignedEnd;
        uint32_t tmp = 0xffffffff;
        memcpy(&tmp, src + size - nb, nb);

        if (!flashWrite(alignedEnd, &tmp, 4)) {
            //NODE_DBG("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
            //    __LINE__, addr, size, alignedBegin, alignedEnd);
            return SPIFFS_ERR_INTERNAL;
        }
    }

    return SPIFFS_OK;
}

s32_t spiffs_hal_erase(u32_t addr, u32_t size) {
    if ((size & (SPI_FLASH_SEC_SIZE - 1)) != 0 ||
        (addr & (SPI_FLASH_SEC_SIZE - 1)) != 0) {
        //NODE_DBG("_spif_erase called with addr=%x, size=%d\r\n", addr, size);
        //abort();
    }
    const uint32_t sector = addr / SPI_FLASH_SEC_SIZE;
    const uint32_t sectorCount = size / SPI_FLASH_SEC_SIZE;
    for (uint32_t i = 0; i < sectorCount; ++i) {
        if (!flashEraseSector(sector + i)) {
            //NODE_DBG("_spif_erase addr=%x size=%d i=%d\r\n", addr, size, i);
            return SPIFFS_ERR_INTERNAL;
        }
    }
    return SPIFFS_OK;
}


spiffs* getFs() {
	return &_fs;
}

static bool _tryMount() {
	u32_t _maxOpenFds = 6;
        spiffs_config config = {0};

        config.hal_read_f       = &spiffs_hal_read;
        config.hal_write_f      = &spiffs_hal_write;
        config.hal_erase_f      = &spiffs_hal_erase;
        config.phys_size        = _size;
        config.phys_addr        = _start;
        config.phys_erase_block = SPI_FLASH_SEC_SIZE;
        config.log_block_size   = _blockSize;
        config.log_page_size    = _pageSize;


        /*if (((uint32_t) std::numeric_limits<spiffs_block_ix>::max()) < (_size / _blockSize)) {
            NODE_DBG("spiffs_block_ix type too small");
            return true;
        }

        if (((uint32_t) std::numeric_limits<spiffs_page_ix>::max()) < (_size / _pageSize)) {
            NODE_DBG("spiffs_page_ix type too small");
            return true;
        }

        if (((uint32_t) std::numeric_limits<spiffs_obj_id>::max()) < (2 + (_size / (2*_pageSize))*2)) {
            NODE_DBG("spiffs_obj_id type too small");
            return true;
        }

        if (((uint32_t) std::numeric_limits<spiffs_span_ix>::max()) < (_size / _pageSize - 1)) {
            NODE_DBG("spiffs_span_ix type too small");
            return true;
        }*/

        // hack: even though fs is not initialized at this point,
        // SPIFFS_buffer_bytes_for_cache uses only fs->config.log_page_size
        // suggestion: change SPIFFS_buffer_bytes_for_cache to take
        // spiffs_config* instead of spiffs* as an argument
        _fs.cfg.log_page_size = config.log_page_size;

        size_t workBufSize = 2 * _pageSize;
        size_t fdsBufSize = SPIFFS_buffer_bytes_for_filedescs(&_fs, _maxOpenFds);
        size_t cacheBufSize = SPIFFS_buffer_bytes_for_cache(&_fs, _maxOpenFds);

        if (!_workBuf) {
            //DEBUGV("SPIFFSImpl: allocating %d+%d+%d=%d bytes\r\n",
            //    workBufSize, fdsBufSize, cacheBufSize,
            //    workBufSize + fdsBufSize + cacheBufSize);
            _workBuf = malloc(workBufSize);
            _fdsBuf = malloc(fdsBufSize);
            _cacheBuf = malloc(cacheBufSize);
        }

        //NODE_DBG("SPIFFSImpl: mounting fs @%x, size=%x, block=%x, page=%x\r\n",
        //    _start, _size, _blockSize, _pageSize);

        int err = SPIFFS_mount(&_fs, &config, _workBuf, _fdsBuf, fdsBufSize, _cacheBuf, cacheBufSize, 0);

        //DEBUGV("SPIFFSImpl: mount rc=%d\r\n", err);

        return err == SPIFFS_OK;
}

int myspiffs_format( void )
{
	if (_size == 0) {
		NODE_DBG("SPIFFS size is zero");
		return 0;
	}

	bool wasMounted = (SPIFFS_mounted(&_fs) != 0);

	if (_tryMount()) {
		SPIFFS_unmount(&_fs);
	}
	int rc = SPIFFS_format(&_fs);
	if (rc != SPIFFS_OK) {
		NODE_DBG("SPIFFS_format: rc=%d, err=%d\r\n", rc, _fs.err_code);
		return 0;
	}

	if (wasMounted) {
		return _tryMount();
	}

	return 1;
}

size_t myspiffs_init() {
        if (SPIFFS_mounted(&_fs) != 0) {
			NODE_DBG("SPIFFS has already been mounted");
            return true;
        }
        if (_size == 0) {
            NODE_DBG("SPIFFS size is zero");
            return 0;
        }
        if (_tryMount()) {
			NODE_DBG("_tryMount return");
            return 1;
        }
        int rc = SPIFFS_format(&_fs);
        if (rc != SPIFFS_OK) {
            //NODE_DBG("SPIFFS_format: rc=%d, err=%d\r\n", rc, _fs.err_code);
            return 0;
        }
        return _tryMount();
}

int myspiffs_open(const char *name, int flags){
  return (int)SPIFFS_open(&_fs, (char *)name, (spiffs_flags)flags, 0);
}

int myspiffs_close( int fd ){
  SPIFFS_close(&_fs, (spiffs_file)fd);
  return 0;
}

size_t myspiffs_write( int fd, const void* ptr, size_t len )
{
  int res = SPIFFS_write(&_fs, (spiffs_file)fd, (void *)ptr, len);
  if (res < 0) {
    //NODE_DBG("write errno %i\n", SPIFFS_errno(&_fs));
    return 0;
  }
  return res;
}

size_t myspiffs_read( int fd, void* ptr, size_t len)
{
  int res = SPIFFS_read(&_fs, (spiffs_file)fd, ptr, len);
  if (res < 0) {
    //NODE_DBG("read errno %i\n", SPIFFS_errno(&_fs));
    return 0;
  }
  return res;
}


void myspiffs_remove( char *name ){
	SPIFFS_remove(&_fs, name);
}

int myspiffs_lseek( int fd, int off, int whence ){
  return SPIFFS_lseek(&_fs, (spiffs_file)fd, off, whence);
}

int myspiffs_eof( int fd ){
  return SPIFFS_eof(&_fs, (spiffs_file)fd);
}

int myspiffs_tell( int fd ){
  return SPIFFS_tell(&_fs, (spiffs_file)fd);
}

int myspiffs_getc( int fd ){
  unsigned char c = 0xFF;
  int res;
  if(!myspiffs_eof(fd)){
    res = SPIFFS_read(&_fs, (spiffs_file)fd, &c, 1);
    if (res != 1) {
      //NODE_DBG("getc errno %i\n", SPIFFS_errno(&_fs));
      return (int)EOF;
    } else {
      return (int)c;
    }
  }
  return (int)EOF;
}

int myspiffs_ungetc( int c, int fd ){
  return SPIFFS_lseek(&_fs, (spiffs_file)fd, -1, SEEK_CUR);
}

int myspiffs_flush( int fd ){
  return SPIFFS_fflush(&_fs, (spiffs_file)fd);
}
int myspiffs_error( int fd ){
  return SPIFFS_errno(&_fs);
}
void myspiffs_clearerr( int fd ){
  SPIFFS_clearerr(&_fs);
}
int myspiffs_rename( const char *old, const char *newname ){
  return SPIFFS_rename(&_fs, (char *)old, (char *)newname);
}
size_t myspiffs_size( int fd ){
  int32_t curpos = SPIFFS_tell(&_fs, (spiffs_file) fd);
  int32_t size = SPIFFS_lseek(&_fs, (spiffs_file) fd, SPIFFS_SEEK_END, 0);
  (void) SPIFFS_lseek(&_fs, (spiffs_file) fd, SPIFFS_SEEK_SET, curpos);
  return size;
}
int myspiffs_check( void )
{
  // ets_wdt_disable();
  // int res = (int)SPIFFS_check(&_fs);
  // ets_wdt_enable();
  // return res;
}

spiffs_DIR *myspiffs_opendir(char *name, spiffs_DIR *d) {
	return SPIFFS_opendir(&_fs, name, d);
}

void esp_spiffs_deinit(u8_t format)
{
    if (SPIFFS_mounted(&_fs)) {
        SPIFFS_unmount(&_fs);
        free(_workBuf);
        free(_fdsBuf);
        free(_cacheBuf);

        if (format) {
            SPIFFS_format(&_fs);
        }
    }
}
void myspiffs_deinit() {
	u8_t format = 1;
	esp_spiffs_deinit(format);
}

s32_t myspiffs_fsinfo(u32_t *total, u32_t *used) {
	return SPIFFS_info(&_fs, total, used);
}