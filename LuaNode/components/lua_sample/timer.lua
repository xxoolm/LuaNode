-- timer
-- tmr.delay(us) 

tmr.delay(1000000);  -- Delay 1000000us, and then output hello
print("hello");

tmr.now(); -- Output timestamp

--[[
func=function()
  print("test");
end
tmr.register(1, 1000, 0, func);  -- output "test" after 1000ms second
tmr.start(1);
tmr.stop(1);
]]