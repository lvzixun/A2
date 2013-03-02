

function func(a)
	local b = 11
	local c = 22
	local d = 33
	return function ()
		local __c1 = 55
		local __c2 = 66
		return function ()
			local dd = 44
			return function ()
				return function ()
					print(dd, a)
				end
			end
		end 
	end
end


func(1)()()
func(2)()()