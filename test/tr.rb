
count = 5000
array = Array.new(count) {|i| count-i}


n  = Time.now.to_i

print "----begin-----\n"
for i in 0..(count-1)
	min = array[i]
	for j in i..(count-1)
		if min>array[j] then
			array[i] = array[j]
			array[j] = min
			min = array[i]
		end	
	end	
end

e = Time.now.to_i

print "---offtime = #{e}-#{n}=#{e-n}----\n"

# array.each_index do |x|
# 	print "#{array[x]}\n"
# end
