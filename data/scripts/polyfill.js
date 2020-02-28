
mat4.toArray = function(mat)
{
	return [mat[0],mat[1],mat[2],mat[3],mat[4],mat[5],mat[6],mat[7],mat[8],mat[9],mat[10],mat[11],mat[12],mat[13],mat[14],mat[15]];
}


Object.defineProperty(Entity.prototype, "position", {
	set: function(v){
		this._position = v;
	},
	get: function(){
		return this._position;
	}
});

Object.defineProperty(Agent.prototype, "position", {
	set: function(v){
		this._position = v;
	},
	get: function(){
		return this._position;
	}
});

Object.defineProperty(Agent.prototype, "model", {
	set: function(v){
		this._model = [
			v[0],
			v[1],
			v[2],
			v[3],
			v[4],
			v[5],
			v[6],
			v[7],
			v[8],
			v[9],
			v[10],
			v[11],
			v[12],
			v[13],
			v[14],
			v[15],
		];
	},
	get: function(){
		return this._model;
	}
});

window.DEG2RAD = 0.0174532925;
/*
//Not working :(
Agent.prototype.properties = new Proxy({},{
	set:function(obj,prop,value){
		console.log(obj, typeof obj)
		obj._setProperty(prop,value);
	},
	get:function(obj,prop){
		return obj._getProperty(prop);		
	}
});*/

