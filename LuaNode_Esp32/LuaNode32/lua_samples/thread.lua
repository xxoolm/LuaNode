-- thread test
-- LED blink

f1=function()
  while true do
    gpio.write(2,0);
    tmr.delay(1);
    gpio.write(2,1);
    tmr.delay(1);
  end
end

th1=thread.start(f1);
