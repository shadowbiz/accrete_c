gameObject = {}

function gameObject:New(x,y,z)
	
	local obj = {};
	
	obj.x = x
	obj.y = y
	obj.z = z
	obj.rot = math.random(360)
	obj.d = math.random(100)

	function obj:setPosition(x,y,z)
		self.x = x
		self.y = y
		self.z = z		
	end

	function obj:setRot(a)
		self.rot = a
	end

	function obj:setX(x)
		self.x = x
	end

	function obj:setY(y)
		self.y = y
	end

	function obj:setZ(z)
		self.z = z
	end

	function obj:getRot()
		return self.rot
	end

	function obj:getX()
		return self.x
	end

	function obj:getY()
		return self.y
	end

	function obj:getZ()
		return self.z
	end

	function obj:getD()
		return self.d
	end

	return obj
end

entity = 0

function init(x, y, z)
	entity = gameObject:New(x, y, z)
end

function setPosition(x, y, z)
	entity:setPosition(x,y,z)
end

function getX()
	return entity:getX()
end

function getY()
	return entity:getY()
end

function getZ()
	return entity:getZ()
end

function update(deltaTime)
	local x = entity:getX()
	local y = entity:getY()
	local rot = entity:getRot()
	local d = entity:getD()

	rot = rot + deltaTime * 10
	if rot > 360 then rot = -360 end

	x = math.cos(math.rad(rot)) * d
	y = math.sin(math.rad(rot)) * d

	--if x > 40 then x = -40 end
	
	entity:setX(x)
	entity:setY(y)
	entity:setRot(rot)
end
