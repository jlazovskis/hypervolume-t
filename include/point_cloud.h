// This file is part of hypervolume-t 

namespace hvt {

	class point_cloud {
		private:
			// The points
			std::vector< hvt::point > points;

			// A list of neighbors for each point
			std::vector< hvt::neighbors > all_neighbors;

			// Distance between pairs of points at given indices (from https://github.com/Ripser)
			float dist(const hvt::index i, const hvt::index j) const {
				assert(i < points.size());
				assert(j < points.size());
				return std::sqrt(
					std::inner_product(
						points[i].begin(),
						points[i].end(),
						points[j].begin(),
						float(),
						std::plus<float>(),
						[](float u, float v) {
							return (u - v) * (u - v);
						}
					)
				);
		  	}

		public:
			// Constructor
			point_cloud() {};

			// (1/3) Declare points by: loading from a file (from https://bitbucket.org/phat-code/phat)
			bool load_points( const std::string filename ) {
				// Check to make sure file can be read
				std::ifstream dummy( filename.c_str() );
				if( dummy.fail() ) {
					std::cerr << "couldn't open file " << filename << std::endl;
					return false;
				}

				// Read in the data (from https://github.com/Ripser)
				this->points = points;
				std::string line;
				std::ifstream input_stream( filename.c_str() );

				hvt::value value;
				while (std::getline(input_stream, line)) {
					hvt::point point;
					std::istringstream s(line);
					while (s >> value) {
						point.push_back(value);
						s.ignore();
					}
					if (!point.empty()) points.push_back(point);
				}
				return true;
			};

			// (2/3) Declare points by: adding barycenters from another point cloud
			bool split_points( hvt::point_cloud initial_point_cloud, std::vector<int>& points_added ) {
				std::vector<int> new_point_count;

				// Check that the input point cloud has points initialized
				assert (initial_point_cloud.get_size() > 0);

				// Populate the points
				int counter1 = 0;
				int counter2 = 0;
				for ( hvt::index i = 0; i < initial_point_cloud.get_size(); i++ ) {
	
					// Temporary containers
					hvt::point current_point;
					hvt::neighbors current_neighbors;
					initial_point_cloud.get_point(i,current_point);
					initial_point_cloud.get_neighbors(i,current_neighbors);

					// Add same point
					hvt::point point;
					for (const hvt::value& value : current_point) { 
						point.push_back(value);
					}
					points.push_back(point);

					// Iterate through neighbors
					for ( int j = 0; j < current_neighbors.size() ; j++ ) {

						// Add midpoint of every two points
						std::vector< hvt::point > neighboring_points;
						hvt::point current_point_b;
						initial_point_cloud.get_point(get_index(current_neighbors[j]),current_point_b);
						neighboring_points.push_back(current_point_b);

						// Declare, compute, record new point
						hvt::point new_point;
						hvt::point_average( current_point, neighboring_points, new_point );
						points.push_back(new_point);
						counter1 += 1;
					
						// Iterate once again for triples
						for ( int k = j+1 ; k < current_neighbors.size() ; k++ ) {

							// Add another neighboring points
							hvt::point current_point_c;
							initial_point_cloud.get_point(get_index(current_neighbors[k]),current_point_c);
							neighboring_points.push_back(current_point_c);

							// Declare, compute, record new point
							hvt::point new_point;
							hvt::point_average( current_point, neighboring_points, new_point );
							points.push_back(new_point);
							counter2 += 1;

							// Drop last element
							neighboring_points.pop_back();
						}
					}
				}

				// Record progress and finish
				new_point_count.push_back(counter1);
				new_point_count.push_back(counter2);
				points_added = new_point_count;
				return true;
			};

			// (3/3) Declare points by: sparsifying another point cloud
			bool sparsify_points( const hvt::point_cloud initial_point_cloud, const hvt::value distance ) {
				// TODO: check that the input point cloud has points initialized
				return true;
			};

			// Compute neighbors at a certain distance (adapted from https://github.com/Ripser)
			// Only find neihgbors at higher indices, to skip duplicates later
			void find_neighbors( const hvt::value threshold ) {

				// Get container for data
				this->all_neighbors = all_neighbors;

				// Iterate
				for ( hvt::index i = 0; i < points.size(); ++i ) {

					// Make temporary neighbor vector
                    std::vector< hvt::index_diameter_t > temp_nb;
					for (hvt::index j = i+1; j < points.size(); ++j) {
						hvt::value d = dist(i, j);
						if (d <= threshold) {
							// std::cout << i << " " << j << "\n";
							temp_nb.push_back({j, d});
						}
					}

					// Register the neighbors
					all_neighbors.push_back(temp_nb);
				}
			};

			// Number of points in point cloud
			hvt::index get_size() {
				return points.size();
			}

			// Point at a given index
			// Sets input reference to requested point
			void get_point( const hvt::index index, hvt::point& my_point ) {
				my_point = points[index];
			}


			// Neighbors of a point at a given index
			// Sets input reference to requested neighbor
			void get_neighbors( const hvt::index index, hvt::neighbors& neighbors ) {
				neighbors = all_neighbors[index];
			}

			// Export to file
			bool export_csv( const std::string filename ) {
				return true;
			}

			// TESTING
			void print_me() {

				// std::cout << dist(10,100);

				// this->points = points;
				// for ( int col = 0; col < points.size(); col++ ) {
				// 	for (const hvt::value& value : points[col]) { 
				// 		std::cout << value << ",";
				// 	}
				// std::cout << "\n";
				// }
				// std::cout << std::endl;
			
				// this->neighbors = neighbors;
				std::cout << all_neighbors.size() << "\n";
				// for ( int col = 0; col < neighbors.size(); col++ ) {
				// 	for (const hvt::index_diameter_t& index_diameter_t : neighbors[col]) { 
				// 		std::cout << get_index(index_diameter_t) << " at " << get_diameter(index_diameter_t) << ",";
				// 	}
				// std::cout << "\n";
				// }
				// std::cout << std::endl;				


			};

	};
}