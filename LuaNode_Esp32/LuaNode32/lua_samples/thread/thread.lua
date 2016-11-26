-- Thread test
-- Start a thread for LED blinking
-- Print status of the thread

f1=function()
  while true do
    gpio.write(2,0);
    tmr.delay(1);
    gpio.write(2,1);
    tmr.delay(1);
  end
end

th1=thread.start(f1);

tmr.delay(2);
status = thread.status(th1);
print(status);	-- this will output running

-- there are still some bugs with suspend methond, chip will restart when it's called
-- thread.suspend(th1);

-- to stop blinking, run the following command:
-- thread.stop(th1)	
