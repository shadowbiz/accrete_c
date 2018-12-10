vec3 = {};

function vec3:New(x,y,z)
	
	local obj = {};
	
	obj.x = x;
	obj.y = y;
	obj.z = z;

	function obj:getX()
		return self.x;
	end

	function obj:getY()
		return self.y;
	end

	function obj:getZ()
		return self.z;
	end

	return obj;
end

function awake()

end

function update(deltaTime)
	vec = vec3:New(1,2,3);
	print(vec:getX(), vec:getY(), vec:getZ());
	--print(vec:getY());
	--print(vec:getZ());
end