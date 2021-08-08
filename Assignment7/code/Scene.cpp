//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection inter = intersect(ray);

    if(inter.happened)
    {
        if(inter.m->hasEmission())// hit source
            return depth == 0 ? inter.m->getEmission() : Vector3f(0.,0.,0.);

        Vector3f L_dir(0.,0.,0.);
        Vector3f L_indir(0.,0.,0.);

        Intersection light_inter;
        float pdf_light=0.0f;
        sampleLight(light_inter,pdf_light);

        auto& N = inter.normal;
        auto& NN = light_inter.normal;

        auto& obj_pos=inter.coords;
        auto& light_pos=light_inter.coords;

        auto diff=light_pos-obj_pos;
        auto light_dir=diff.normalized();
        float dist_sqr=diff.x*diff.x+diff.y*diff.y+diff.z*diff.z;

        Ray light(obj_pos,light_dir);
        Intersection light2obj=intersect(light);

        if(light2obj.happened && (light2obj.coords-light_pos).norm() < 1e-2)
        {
            Vector3f f_r = inter.m->eval(ray.direction, light_dir, N);
            L_dir = light_inter.emit * f_r * dotProduct(N,light_dir) * dotProduct(NN,-light_dir) / dist_sqr / pdf_light;
        }

        if(get_random_float()<RussianRoulette)
        {
            Vector3f next_dir=inter.m->sample(ray.direction,N).normalized();
            Ray next_ray(obj_pos,next_dir);
            Intersection next_inter = intersect(next_ray);
            if(next_inter.happened && !next_inter.m->hasEmission())
            {
                float pdf = inter.m->pdf(ray.direction,next_dir,N);
                Vector3f f_r = inter.m->eval(ray.direction, next_dir, N);
                L_indir = castRay(next_ray, depth+1) * f_r * dotProduct(N, next_dir) / pdf / RussianRoulette;
            }
        }
        return L_dir + L_indir;
    }
    return Vector3f(0.,0.,0.);
}