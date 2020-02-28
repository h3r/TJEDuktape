/*
* David Moreno - UPF
*/

function Facade ()
{
	
}

/* 
* Receives as a parameter a game/system entity, a scene node which is being evaluated
* Returns a vec3 with the position
*/
Facade.prototype.getEntityPosition = function( entity )
{
	var c_entity = getEntity(entity.id);
	return c_entity.position;
	console.warn("getEntityPosition() Must be implemented to use HBTree system");
}


/*
* Returns an Array of the existing entities in the scene
* The type of the entity is irrelevant
*/
Facade.prototype.getListOfAgents = function(  ){
	return getAllAgents();
}

Facade.prototype.entityHasProperty = function( entity, property ){
	var c_entity = getEntity(entity.id);
	return c_entity.hasProperty( property );
}

Facade.prototype.getEntityPropertyValue = function( name , entity ){	
	var c_entity = getEntity(entity.id);
	return c_entity.getProperty(name);
}

Facade.prototype.setEntityProperty = function( entity, property, value ){
	var c_entity = getEntity(entity.id);
	return c_entity.setProperty(property, value);
}

/*
* Check if a concrete entity is in a certain position
* The entity must have a global position (or the possibility to access to it)
* The target can be a vec3 directly or an object containing the position of the target
*/
Facade.prototype.entityInTarget = function( enitity, target, threshold)
{
	return vec3.distance(entity.position, target ) <= threshold;
	console.warn("entityInTarget() Must be implemented to use HBTree system");
}


/*
* @entity: the virtual entity evaluated. The type you are using as an entity 
* @look_at_pos: vec3 with the target position to check if it's seen or not 
* @limit_angle: a number in degrees (field of view)
*/
Facade.prototype.canSeeElement = function( entity, look_at_pos, limit_angle )
{
	var front = entity.model.front;
	var dir = vec3.sub(vec3.create(), look_at_pos, entity.position );
	var angle = Math.arccos( vec3.dot(front, dir) ) * DEG2RAD;
	return (angle <= limit_angle);
}

//For the Patrol Node
/*
* Check and find the next control point of a path (to patrol)
* If not path, return false
*/
Facade.prototype.checkNextTarget = function( enitity )
{

	console.warn("entityInTarget() Must be implemented to use HBTree system");
}

//For the EQSNearestInterestPoint Node
/*
* Return all the existing interest points
*/
Facade.prototype.getInterestPoints = function(  )
{
	
	console.warn("entityInTarget() Must be implemented to use HBTree system");
}









// var facade = new Facade();
// function overrideFacade( facade )
// {
// 	facade.getEntityPosition = function( entity )
// 	{
// 		if(entity)
// 		{
// 			return 	entity.scene_node.getGlobalPosition();
// 		}
// 	}
// 	facade.setEntityProperty = function (entity, name, value) 
// 	{
// 		entity.properties[name] = value;
// 	}

// 	facade.getEntityPropertyValue = function( name , entity)
// 	{	
// 		var value = null;
// 		// this.title is the name of the property to search
// 		if(name == "position")
// 		{
// 			value = this.getEntityPosition( entity );
// 			return value;
// 		}
// 		else
// 		{
// 			//Get properties from the app
// 			if(CORE.Scene.bprops.includes(name))
// 			{
// 				value = CORE.Scene.zones["zone1"][name];
// 				return value;
// 			}
// 			//Get properties from the agent
// 			else if(entity)
// 			{
// 				value = entity.properties[name];
// 				return value;
// 			}
// 		}
// 	}

// 	facade.getListOfAgents = function(info)
// 	{
// 		if(CORE.AgentManager.agents)
// 			return CORE.AgentManager.agents;
// 		else
// 			return null;
// 	}

// 	facade.entityInTarget = function( entity, target_, threshold)
// 	{
// 		if(!target_ || isEmpty(target_))
// 			return false;
// 		if(target_.constructor == Float32Array)
// 			var target = target_;
// 		else
// 			var target = target_.constructor == Array ? target_ : target_.position;
		
// 		if(!target) target = entity.scene_node.position;
// 		var current_pos = []; 
// 		current_pos[0] = entity.scene_node.getGlobalPosition()[0];
// 		current_pos[1] = entity.scene_node.getGlobalPosition()[2];

// 		var a = vec2.fromValues(current_pos[0],current_pos[1]);
// 		var b = vec2.fromValues(target[0],target[2]);
		
// 		var dist = vec2.distance(a,b);
// 		// console.log("dist", dist);

// 		if(dist < threshold)
// 		{
// 			for(var i  in entity.path)
// 				if(entity.path[i] && entity.path[i].id == target.id)
// 					entity .path[i].visited = true;
			
// 			return true;
// 		} 
// 		return false;
// 	}

// 	facade.checkNextTarget = function( entity )
// 	{
// 		if(entity.checkNextTarget())
// 		{
// 			entity.in_target = false;
// 			return true;  
// 		}
// 		return false;
// 	}

// 	facade.entityHasProperty = function( entity, property )
// 	{
// 		if(entity.properties[property])
// 			return true;
// 	}

// 	facade.getInterestPoints = function()
// 	{
// 		return CORE.Scene.properties.interest_points;
// 	}

// 	facade.canSeeElement = function( entity , lookat, degrees)
// 	{
// 		if(entity.canSeeElement(lookat, degrees))
// 			return true;
// 		return false;
// 	}

// 	facade.applyTargetProperties = function( target_properties, entity )
// 	{
// 		CORE.Scene.applyTargetProperties(entity.properties.target, entity);
// 	}

// 	facade.setEntityProperty = function (entity, property, value)
// 	{
// 		entity.properties[property] = value;
// 	}

// 	facade.getAnimation = function( filename )
// 	{
// 		return animation_manager.animations[filename.toLowerCase()];
// 	}

// }
