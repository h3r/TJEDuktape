#pragma once
#include "includes.h"
#include "game.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"


class Entity {

protected:

    typedef enum {
        BASE = 0,
        AGENT,
    } EType;

    static inline unsigned int count = 0;
    static inline std::vector<Entity*> all_entities;

public:

    const unsigned int id;
    const EType type;
    Vector3 position;

    Entity() :
        id(++count),
        type(BASE)
    {
        all_entities.push_back(this);
    }

    Entity(EType type) :
        id(++count),
        type(type)
    {
        all_entities.push_back(this);
    }


    static Entity& getEntity(const unsigned int ID) { 
        return *all_entities[ID]; 
    }
    unsigned int getID() { return id; }
    std::vector<float>  getPosition() { return { position.x,position.y,position.z }; }
    void      setPosition(const std::vector<float> v) {
        position.x = v[0];
        position.y = v[1];
        position.z = v[2];
    }
};


class Agent : public Entity {

    std::vector<Vector3> path;
    std::vector<Vector3> interest_points;
    std::unordered_map<std::string, DukValue> properties;

public:
    mat4 model;
    vec4 color = vec4(1, 0, 1, 1);
    Mesh* mesh;
    Shader* shader;
    Texture* texture;

    Agent() :
        Entity(AGENT)
    {
        texture = new Texture();
        texture->load("data/texture.tga");
        mesh = Mesh::Get("data/box.ASE");
        shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    }

    static Agent getAgent(const unsigned int ID) {
        return *reinterpret_cast<Agent*>(all_entities[ID]);
    }

    static std::vector<Agent>getAllAgents() {
        std::vector<Agent> all_agents;
        for (auto* e : Entity::all_entities) {
            if (e->type == AGENT) {
                Agent a = *(reinterpret_cast<Agent*>(e));
                all_agents.push_back(a);
            }
        }
        return all_agents;
    }


    const DukValue getProperty(std::string key) { return  properties[key]; }
    void setProperty(std::string key, const DukValue& value) { properties[key] = value; }
    bool hasProperty(std::string key) { return properties.count(key); }

    std::vector<float>  getPosition() { 
        vec3 translation = model.getTranslation();
        return { translation.x,translation.y,translation.z };
    }
    void setPosition(const std::vector<float> v) {
        model.setTranslation(v[0], v[1], v[2]);
    }

    std::vector<float>  getModel() {
        std::vector<float> out(model.m, model.m + 16);
        return std::move(out);
    }
    void setModel(const std::vector<float> v) {
        std::copy(v.begin(), v.end(), model.m);
    }


    void render() {
        shader->enable();
        shader->setUniform("u_color", color);
        shader->setUniform("u_texture", texture);
        mesh->render(GL_TRIANGLES);
    }

};