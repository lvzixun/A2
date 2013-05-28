

function func()
	local a = 1123
	local b = function ()
		print(a)
		a = 'xxxx'
	end

	local c = function ()	
		print(a)
		a = 'bbb'
	end

	return b, c
end



a, b = func()

a()
b()