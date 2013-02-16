
count = 5000
array = Array.new(count) {|i| count-i}


n  = Time.now.to_i

print "----begin-----\n"
i=0
while i<count do
	min = array[i]
	j = i
	while j<count do
		if min>array[j] then
			array[i] = array[j]
			array[j] = min
			min = array[i]
		end
		j += 1			
	end
	i += 1	
end

e = Time.now.to_i

print "---offtime = #{e}-#{n}=#{e-n}----\n"

# array.each_index do |x|
# 	print "#{array[x]}\n"
# end
