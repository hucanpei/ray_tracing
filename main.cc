#include<iostream>
#include<float.h>
#include<functional>
#include<tbb/tbb.h>
#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>
#include"sphere.h"
#include"hitable_list.h"
#include"camera.h"
#include"material.h"
using namespace std;

vec3 color(const ray &r,hitable *world,int depth){
    hit_record rec;
    if(world->hit(r,0.001,MAXFLOAT,rec)){
        ray scattered;
        vec3 attenuation;
        if(depth<50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return attenuation*color(scattered,world,depth+1);
        }else{
            return vec3(0,0,0);
        }
    }else{
        vec3 unit_direction=unit_vector(r.direction());
        float t=0.5*(unit_direction.y()+1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0)+t*vec3(0.5,0.7,1.0);
    }
}

hitable *random_scene(){
    int n=500;
    hitable **list=new hitable*[n+1];
    list[0]=new sphere(vec3(0,-1000,0),1000,new lambertian(vec3(0.5,0.5,0.5)));
    int i=1;
    for(int a=-11;a<11;++a){
        for(int b=-11;b<11;++b){
            float choose_mat=drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
            if((center-vec3(4,0.2,0)).length()>0.9){
                if(choose_mat<0.8)
                    list[i++]=new sphere(center,0.2,new lambertian(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48())));
                else if(choose_mat<0.95)
                    list[i++]=new sphere(center,0.2,new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48())),0.5*drand48()));
                else
                    list[i++]=new sphere(center,0.2,new dielectric(1.5));
            }
        }
    }
    list[i++]=new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
    list[i++]=new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(0.4,0.2,0.1)));
    list[i++]=new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5),0.0));
    return new hitable_list(list,i);
}


int main(){
    int nx=1200,ny=nx*9.0/16.0,ns=30;
    cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n";

    hitable *world=random_scene();

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus=(lookfrom-lookat).length();
    float aperture=0.1;
    camera cam(lookfrom,lookat,vec3(0,1,0),20,float(nx)/float(ny),aperture,dist_to_focus);

    vector<vector<vector<int>>> out_img(ny,vector<vector<int>>(nx,vector<int>(3,0)));

    auto parallel_f=[&](int j_rev){
        int j=ny-1-j_rev;
        for(int i=0;i<nx;++i){
            vec3 col(0,0,0);
            for(int s=0;s<ns;++s){
                float u=float(i+drand48())/float(nx);
                float v=float(j+drand48())/float(ny);
                ray r=cam.get_ray(u,v);
                vec3 p=r.point_at_parameter(2.0);
                col+=color(r,world,0);
            }
            col/=float(ns);
            col=vec3(sqrt(col.x()),sqrt(col.y()),sqrt(col.z()));
            int ir=int(255.99*col.x());
            int ig=int(255.99*col.y());
            int ib=int(255.99*col.z());
            out_img[j_rev][i][0]=ir;
            out_img[j_rev][i][1]=ig;
            out_img[j_rev][i][2]=ib;
            //cout<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
        //cout<<j<<"\n";
    };

    tbb::parallel_for(0,ny,1,parallel_f);

    for(auto &line:out_img){
        for(auto &elem:line){
            cout<<elem[0]<<" "<<elem[1]<<" "<<elem[2]<<"\n";
        }
    }

}
