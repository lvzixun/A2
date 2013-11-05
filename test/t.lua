local mt = require "m_time"

local t = {}
for i=1,5000 do
	table.insert(t, 5000-i)
end

local s = mt.ms_time()
print("-----begin sort--------")
for i=1,5000 do
	local min = t[i]
	for j=i,5000 do
		if min>t[j] then
			min = t[j]
			t[j] = t[i]
			t[i] = min
		end
	end
end

print("------end sort --------")
local e = mt.ms_time()
print("total time(ms): ", e-s)
