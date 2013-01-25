

local t = {}
for i=1,5000 do
	table.insert(t, 5000-i)
end

local s = t_time()
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

local e = t_time()

print((e-s)/1000000 )