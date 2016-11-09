-- file
-- To read any file, you should open it first 

file.open("myfile.lua","r");  -- Open an existing file, named myfile.lua. If the file doesn't exist, open failed
content = file.read(10);  -- Read 10 characters from the open file, and save them to content
print(content);
file.close();  -- Close the file

--[[
file.open("myfile.lua","w+")  -- Open and append content to myfile.lua
file.write("hello");
file.close();
]]

--[[
file.open("myfile.lua","w")  -- Open file myfile.lua. If it doesn't exist, create it, otherwise, overwrite the file
file.write("hello");
file.close();
]]

--[[
file.remove("myfile.lua");  -- remove myfile.lua
]]

--[[
info = file.list();  -- List all files stored in flash
for k,v in pairs(info) do
  l = string.format(\"%-15s\",k);
  print(l..\" : \"..v..\" bytes\");
end
]]

--[[
file.format();  -- Format flash, will remove all files stored in flash!
]]