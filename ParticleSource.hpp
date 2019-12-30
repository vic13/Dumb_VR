
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    int ttl; //time to live
};

class ParticleSource {
public:
    int nbParticleGen = 1;
    float particleSpeed = 0.004;
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
        // Gen new particles
        for (int i=0; i<nbParticleGen; i++) {
            Particle p;
            p.position = this->position;
            p.direction = randomDirection();
            p.ttl = this->initTTL;
            this->particles.push_back(p);
        }
        
        // Update particles
        for (unsigned int i=0; i<this->particles.size(); i++) {
            this->particles[i].position += particleSpeed*this->particles[i].direction;
            this->particles[i].direction = glm::normalize((1.0f-upSpeed)*this->particles[i].direction + upSpeed*glm::vec3(0.0f,1.0f,0.0f));
            float lifeRatio = ((this->initTTL-this->particles[i].ttl)/(float)this->initTTL);
            this->particles[i].color = lifeRatio*endColor + (1-lifeRatio)*initColor;
            if (this->particles[i].ttl == 0) {
                this->particles.erase(this->particles.begin());
            } else {
                this->particles[i].ttl = this->particles[i].ttl - 1;
            }
        }
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
