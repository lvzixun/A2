
function func(a, b)
	return function ()
		return function ()
			print(a, b)
			a, b = 11, 22
		end
	end
end

local f = func(1, 2)

f()()

f()()