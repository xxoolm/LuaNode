-- connect GPIO18 with GPIO25
-- connect GPIO19 with GPIO26
i2c.setup(i2c.SLAVE,i2c.I2C_0,26,25,40)
i2c.setup(i2c.MASTER,i2c.I2C_1,19,18)
-- slave write something, master read
i2c.write(i2c.SLAVE,i2c.I2C_0,40,"helloworld")
c=i2c.read(i2c.MASTER,i2c.I2C_1,40,10)
print(c)

tmr.delay(5)
-- master write something, slave read
i2c.write(i2c.MASTER,i2c.I2C_1,40,"test")
c=i2c.read(i2c.SLAVE,i2c.I2C_0,40,4)
print(c)