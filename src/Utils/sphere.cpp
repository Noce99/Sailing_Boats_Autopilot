#include <vector>
#include <glm/glm.hpp>

class Cube{
    protected:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normalVecs;
    std::vector<glm::vec3> single_vertices;
    int num_of_vertices = 0;
    int num_of_single_vertices = 0;
    public:
    Cube(double side_size, int points_for_side){
        double trg_side = side_size/(1.0*points_for_side);
        double x, y, z;
        x = side_size/2;
        for (int iy = 0; iy < points_for_side; iy++){
            for (int iz = 0; iz < points_for_side; iz++){
                y = (side_size*(1.0*iy)/(1.0*points_for_side))-side_size/2;
                z = (side_size*(1.0*iz)/(1.0*points_for_side))-side_size/2;
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                vertices.push_back(glm::vec3(x,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y+trg_side,z+trg_side));
                vertices.push_back(glm::vec3(x,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                normalVecs.push_back(glm::vec3(1,0,0));
                normalVecs.push_back(glm::vec3(1,0,0));
                normalVecs.push_back(glm::vec3(1,0,0));
                normalVecs.push_back(glm::vec3(1,0,0));
                normalVecs.push_back(glm::vec3(1,0,0));
                normalVecs.push_back(glm::vec3(1,0,0));
                num_of_vertices += 6;

                single_vertices.push_back(glm::vec3(x,y,z));
                single_vertices.push_back(glm::vec3(x,y+trg_side,z));
                single_vertices.push_back(glm::vec3(x,y,z+trg_side));
                single_vertices.push_back(glm::vec3(x,y+trg_side,z+trg_side));
                num_of_single_vertices += 4;
            }
        }
        x = -side_size/2;
        for (int iy = 0; iy < points_for_side; iy++){
            for (int iz = 0; iz < points_for_side; iz++){
                y = (side_size*(1.0*iy)/(1.0*points_for_side))-side_size/2;
                z = (side_size*(1.0*iz)/(1.0*points_for_side))-side_size/2;
                vertices.push_back(glm::vec3(x,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                vertices.push_back(glm::vec3(x,y+trg_side,z+trg_side));
                vertices.push_back(glm::vec3(x,y,z+trg_side));
                normalVecs.push_back(glm::vec3(-1,0,0));
                normalVecs.push_back(glm::vec3(-1,0,0));
                normalVecs.push_back(glm::vec3(-1,0,0));
                normalVecs.push_back(glm::vec3(-1,0,0));
                normalVecs.push_back(glm::vec3(-1,0,0));
                normalVecs.push_back(glm::vec3(-1,0,0));
                num_of_vertices += 6;

                single_vertices.push_back(glm::vec3(x,y,z));
                single_vertices.push_back(glm::vec3(x,y+trg_side,z));
                single_vertices.push_back(glm::vec3(x,y,z+trg_side));
                single_vertices.push_back(glm::vec3(x,y+trg_side,z+trg_side));
                num_of_single_vertices += 4;
            }
        }
        z = side_size/2;
        for (int ix = 0; ix < points_for_side; ix++){
            for (int iy = 0; iy < points_for_side; iy++){
                x = (side_size*(1.0*ix)/(1.0*points_for_side))-side_size/2;
                y = (side_size*(1.0*iy)/(1.0*points_for_side))-side_size/2;
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y+trg_side,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                num_of_vertices += 6;

                single_vertices.push_back(glm::vec3(x,y,z));
                single_vertices.push_back(glm::vec3(x,y+trg_side,z));
                single_vertices.push_back(glm::vec3(x+trg_side,y,z));
                single_vertices.push_back(glm::vec3(x+trg_side,y+trg_side, z));
                num_of_single_vertices += 4;
            }
        }
        z = -side_size/2;
        for (int ix = 0; ix < points_for_side; ix++){
            for (int iy = 0; iy < points_for_side; iy++){
                x = (side_size*(1.0*ix)/(1.0*points_for_side))-side_size/2;
                y = (side_size*(1.0*iy)/(1.0*points_for_side))-side_size/2;
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y+trg_side,z));
                vertices.push_back(glm::vec3(x,y+trg_side,z));
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y+trg_side,z));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                num_of_vertices += 6;

                single_vertices.push_back(glm::vec3(x,y,z));
                single_vertices.push_back(glm::vec3(x,y+trg_side,z));
                single_vertices.push_back(glm::vec3(x+trg_side,y,z));
                single_vertices.push_back(glm::vec3(x+trg_side,y+trg_side, z));
                num_of_single_vertices += 4;
            }
        }
        y = side_size/2;
        for (int ix = 0; ix < points_for_side; ix++){
            for (int iz = 0; iz < points_for_side; iz++){
                x = (side_size*(1.0*ix)/(1.0*points_for_side))-side_size/2;
                z = (side_size*(1.0*iz)/(1.0*points_for_side))-side_size/2;
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z+trg_side));
                vertices.push_back(glm::vec3(x+trg_side,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y,z));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                normalVecs.push_back(glm::vec3(0,0,1));
                num_of_vertices += 6;

                single_vertices.push_back(glm::vec3(x,y,z));
                single_vertices.push_back(glm::vec3(x,y,z+trg_side));
                single_vertices.push_back(glm::vec3(x+trg_side,y,z));
                single_vertices.push_back(glm::vec3(x+trg_side,y, z+trg_side));
                num_of_single_vertices += 4;
            }
        }
        y = -side_size/2;
        for (int ix = 0; ix < points_for_side; ix++){
            for (int iz = 0; iz < points_for_side; iz++){
                x = (side_size*(1.0*ix)/(1.0*points_for_side))-side_size/2;
                z = (side_size*(1.0*iz)/(1.0*points_for_side))-side_size/2;
                vertices.push_back(glm::vec3(x,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z));
                vertices.push_back(glm::vec3(x+trg_side,y,z+trg_side));
                vertices.push_back(glm::vec3(x+trg_side,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y,z+trg_side));
                vertices.push_back(glm::vec3(x,y,z));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                normalVecs.push_back(glm::vec3(0,0,-1));
                num_of_vertices += 6;

                single_vertices.push_back(glm::vec3(x,y,z));
                single_vertices.push_back(glm::vec3(x,y,z+trg_side));
                single_vertices.push_back(glm::vec3(x+trg_side,y,z));
                single_vertices.push_back(glm::vec3(x+trg_side,y, z+trg_side));
                num_of_single_vertices += 4;
            }
        }
    }
    std::vector<glm::vec3> getVertices(){
        return vertices;
    }
    std::vector<glm::vec3> getNormalVecs(){
        return normalVecs;
    }
    int getNumVertices(){
        return num_of_vertices;
    }
    int getNumSingleVertices(){
        return num_of_single_vertices;
    }
    glm::vec3 get_index(int i){
        return single_vertices[i];
    }
};

class Sphere: public Cube{
    public:
    Sphere(double side_size, int points_for_side):Cube(side_size, points_for_side){
    	double norm;
        double x2, y2, z2;
        for (int i=0; i<num_of_vertices; i++){
            x2 = vertices[i].x*vertices[i].x;
            y2 = vertices[i].y*vertices[i].y;
            z2 = vertices[i].z*vertices[i].z;
            
            vertices[i].x = vertices[i].x*sqrt(1-y2/2-z2/2+y2*z2/3);
            vertices[i].y = vertices[i].y*sqrt(1-x2/2-z2/2+x2*z2/3);
            vertices[i].z = vertices[i].z*sqrt(1-y2/2-x2/2+y2*x2/3);
            
            norm = sqrt(vertices[i].x*vertices[i].x+vertices[i].y*vertices[i].y+vertices[i].z*vertices[i].z);
            
            vertices[i].x = vertices[i].x/norm;
            vertices[i].y = vertices[i].y/norm;
            vertices[i].z = vertices[i].z/norm;
            
            normalVecs[i].x = vertices[i].x;
            normalVecs[i].y = vertices[i].y;
            normalVecs[i].z = vertices[i].z;
            if (i<num_of_single_vertices){
                x2 = single_vertices[i].x*single_vertices[i].x;
            	y2 = single_vertices[i].y*single_vertices[i].y;
            	z2 = single_vertices[i].z*single_vertices[i].z;

                single_vertices[i].x = single_vertices[i].x*sqrt(1-y2/2-z2/2+y2*z2/3);
            	single_vertices[i].y = single_vertices[i].y*sqrt(1-x2/2-z2/2+x2*z2/3);
            	single_vertices[i].z = single_vertices[i].z*sqrt(1-y2/2-x2/2+y2*x2/3);
            	
            	norm = sqrt(single_vertices[i].x*single_vertices[i].x+single_vertices[i].y*single_vertices[i].y+single_vertices[i].z*single_vertices[i].z);
            
		        single_vertices[i].x = single_vertices[i].x/norm;
		        single_vertices[i].y = single_vertices[i].y/norm;
		        single_vertices[i].z = single_vertices[i].z/norm;
            }
        }
    }
};
