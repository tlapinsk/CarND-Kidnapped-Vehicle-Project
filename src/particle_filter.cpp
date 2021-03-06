/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *  Author: Tiffany Huang
 */
#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

#define num_particles 100

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 
	// Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).
	double std_x, std_y, std_theta, weight;
	std_x     = std[0];
	std_y     = std[1];
	std_theta = std[2];
	default_random_engine gen;

	// Normal distribution for x, y, and theta
	normal_distribution<double> dist_x(x, std_x);
	normal_distribution<double> dist_y(y, std_y);
	normal_distribution<double> dist_theta(theta, std_theta);
	
	// Generate particles
	for (int i = 0; i < num_particles; ++i) {
		x     = dist_x(gen);
		y     = dist_y(gen);
		theta = dist_theta(gen);
		
		Particle particle;
		particle.id     = i;
		particle.x      = x;
		particle.y      = y;
		particle.theta  = theta;
		particle.weight = 1.0;
		
		particles.push_back(particle);
		weights.push_back(particle.weight);
	}
	is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/
	double std_x, std_y, std_theta;
	std_x     = std_pos[0];
	std_y     = std_pos[1];
	std_theta = std_pos[2];
	default_random_engine gen;

	// Check is yaw_rate is zero
	for (int i = 0; i < num_particles; ++i) 
	{
		if(abs(yaw_rate) != 0) 
		{
			// Add measurements to particles
			particles[i].x     += (velocity/yaw_rate) * (sin(particles[i].theta + (yaw_rate * delta_t)) - sin(particles[i].theta));
      particles[i].y     += (velocity/yaw_rate) * (cos(particles[i].theta) - cos(particles[i].theta + (yaw_rate * delta_t)));
      particles[i].theta += yaw_rate * delta_t;
		} else 
		{
			// Add measurements to particles
      particles[i].x += velocity * delta_t * cos(particles[i].theta);
      particles[i].y += velocity * delta_t * sin(particles[i].theta);
		}
		// Normal distributions for x, y, theta
	 	normal_distribution<double> dist_x(particles[i].x, std_x);
		normal_distribution<double> dist_y(particles[i].y, std_y);
		normal_distribution<double> dist_theta(particles[i].theta, std_theta);

		// Add noise
	 	particles[i].x     = dist_x(gen);
	 	particles[i].y     = dist_y(gen);
	 	particles[i].theta = dist_theta(gen);
	}
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.
	for (auto &observation : observations) 
	{
		double minDist = std::numeric_limits<double>::max();
		for (int i = 0; i < predicted.size(); i++) 
		{
			const double distance = dist(observation.x, observation.y, predicted[i].x, predicted[i].y);
			if (distance < minDist)
			{
				minDist = distance;
				observation.id = i;
			}
		}
	}
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html
  // Range and bearing standard deviation
  double std_r, std_b;
	std_r = std_landmark[0];
	std_b = std_landmark[1];
	weights.clear();

	// Loop over particles
	for (int i = 0; i < num_particles; ++i)
	{	
		double particle_x     = particles[i].x;
		double particle_y     = particles[i].y;
		double particle_theta = particles[i].theta;
		
		Map selected_map;
		selected_map.landmark_list.clear();
		int idx_landmark = 0;
		
		// Loop over landmarks
		for (int j = 0; j < map_landmarks.landmark_list.size(); ++j)
		{	
			if (abs(map_landmarks.landmark_list[j].x_f - particle_x) <= sensor_range && 
				abs(map_landmarks.landmark_list[j].y_f - particle_y) <= sensor_range)
			{
				selected_map.landmark_list.push_back(map_landmarks.landmark_list[j]);
			}
		}

		vector<LandmarkObs> transformed_obs;
		LandmarkObs temp_ob;

		// Loops over observations
		for (int j = 0; j < observations.size(); ++j)
		{	
			temp_ob.x = observations[j].x * cos(particle_theta) - observations[j].y * sin(particle_theta) + particle_x;
			temp_ob.y = observations[j].x * sin(particle_theta) + observations[j].y * cos(particle_theta) + particle_y;

			double distance = pow(selected_map.landmark_list[0].x_f - temp_ob.x, 2) + pow(selected_map.landmark_list[0].y_f-temp_ob.y, 2);
			double temp_dist;
			temp_ob.id = 0;
			for (int k = 1; k < selected_map.landmark_list.size(); ++k)
			{
				temp_dist = pow(selected_map.landmark_list[k].x_f - temp_ob.x,2) + pow(selected_map.landmark_list[k].y_f-temp_ob.y,2);
				if (temp_dist < distance)
				{
					distance = temp_dist;
					temp_ob.id = k;
				}
			}
			transformed_obs.push_back(temp_ob);
		}

		// Update weights
		double new_weights = 1.0;
		double num, denom;
		for (int j = 0; j < transformed_obs.size(); ++j)
		{	
			num = exp(-pow(transformed_obs[j].x - selected_map.landmark_list[transformed_obs[j].id].x_f, 2)/2.0/pow(std_r, 2)
						-pow(transformed_obs[j].y - selected_map.landmark_list[transformed_obs[j].id].y_f, 2)/2.0/pow(std_b,2));
			denom = 2.0/M_PI/std_r/std_b;
			new_weights *= num/denom;
		}
		weights.push_back(new_weights);
	}
}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
	vector<Particle> selected_particles;
	default_random_engine gen;
	
	discrete_distribution<> distribution (weights.begin(), weights.end());

	for (int i = 0; i < num_particles; ++i)
	{
		int x = distribution(gen);
		selected_particles.push_back(particles[x]);
	}
	particles = selected_particles;
}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations, 
                                     const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
  //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations = associations;
  particle.sense_x      = sense_x;
  particle.sense_y      = sense_y;

  return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}