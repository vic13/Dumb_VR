
#include <vector>
#include <chrono> 

struct Particle {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    int ttl; //time to live
};

class ParticleSource {
public:
    int nbParticleGen = 1;
    float particleSpeed = 0.01;
    float upSpeed = 0.1;
    int initTTL = 2*60;
    glm::vec3 initColor;
    glm::vec3 endColor;
    glm::vec3 position;
    std::vector<Particle> particles;
    
    
    ParticleSource(glm::vec3 position, glm::vec3 initColor, glm::vec3 endColor) {
        this->position = position;
        this->initColor = initColor;
        this->endColor = endColor;
    }
    
    std::vector<Particle> getParticles() {
        return this->particles;
    }
    
    void updateParticles() {
        auto start = std::chrono::high_resolution_clock::now();

        // Gen new particles
        for (int i=0; i<nbParticleGen; i++) {
            Particle p;
            p.position = this->position;
            p.direction = randomDirection();
            p.ttl = this->initTTL;
            this->particles.push_back(p);
        }

        auto middle = std::chrono::high_resolution_clock::now();
        
        // Update particles
        for (unsigned int i=0; i<this->particles.size(); i++) {
            Particle* p = &(this->particles[i]);
            p->position += particleSpeed*p->direction;
            p->direction = glm::normalize((1.0f-upSpeed)*p->direction + upSpeed*glm::vec3(0.0f,1.0f,0.0f));
            float lifeRatio = ((this->initTTL-p->ttl)/(float)this->initTTL);
            p->color = lifeRatio*endColor + (1-lifeRatio)*initColor;
            if (p->ttl == 0) {
                this->particles.erase(this->particles.begin());
            } else {
                p->ttl = p->ttl - 1;
            }
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto durationTotal = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        auto durationMiddle = std::chrono::duration_cast<std::chrono::microseconds>(middle - start);
        //std::cout << "elapsed time total: " <<  durationTotal.count() << " middle: " << durationMiddle.count() << std::endl;
    }
    
private:
    glm::vec3 randomDirection() {
        float randomX = randomValue();
        float randomY = randomValue();
        float randomZ = randomValue();
        return glm::normalize(glm::vec3(randomX, randomY, randomZ));
    }
    
    float randomValue() {
        return 2.0f*((float) rand()/RAND_MAX)-1.0f;
    }
    
};
