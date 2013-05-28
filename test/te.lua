function func()
	local a = 1123
	local fo1 = function ()
		a = 4456
	end

	fo1()
	print(a)
end

func()