// This file is part of hypervolume-t 
// Header file for the point_cloud class

namespace hvt {

	// Dimension and point types
	typedef CGAL::Epick_d< CGAL::Dynamic_dimension_tag > kernel;
	typedef typename kernel::Point_d Point_d;	

	class point_cloud {

		private:
			// The points
			std::vector< hvt::point > points;

			// A list of neighbors for each point
			std::vector< hvt::neighbors > all_neighbors;

			// Distance between pairs of points at given indices
			// Modified from from https://github.com/Ripser
			float dist( const hvt::index i, const hvt::index j ) const {
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

			// Input / output
			// Modified from https://bitbucket.org/phat-code/phat
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

			void export_points( const std::string filename ) {
  				
				// Set up file for writing
  				std::ofstream outfile;
  				outfile.open(filename);

  				// Write headers
  				outfile << "x0";
				for ( int i = 1; i < get_dim(); i++ ) {
  					outfile << ",x" << i;
				}
				outfile << "\n";

				// Write values
				for ( const hvt::point& point : points ) { 
  					outfile << point[0];
					for ( int i = 1; i < get_dim(); i++ ) {
	  					outfile << "," << point[i];
	  				}
	  				outfile << "\n";
	  			}

	  			// Close file and exit
				outfile.close();
			}

			// Compute neighbors at a certain distance (adapted from https://github.com/Ripser)
			// Only find neighbors at higher indices, to skip duplicates later
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

			// Add a point to the end of the list of points
			void add_point( hvt::point new_point ) {
				points.push_back(new_point);
			}

			// Set all points at once
			void add_points( std::vector< hvt::point >& new_points ) {
				points = new_points;
			}

			// Dimension of points in point cloud
			hvt::index get_dim() {
				return points[0].size();
			}

			// Number of points in point cloud
			hvt::index get_size() {
				return points.size();
			}

			// Point at a given index
			// Sets input reference to requested point
			void get_point( const hvt::index index, hvt::point& my_point ) {
				my_point = points[index];
			}

			// Point at a given index, as CGAL point
			Point_d get_point_asCGAL( const hvt::index index, hvt::point& my_point, hvt::index my_dim ) {
				my_point = points[index];
				return Point_d(my_dim, my_point.begin(), my_point.end());
			}

			// Neighbors of a point at a given index
			// Sets input reference to requested neighbor
			void get_neighbors( const hvt::index index, hvt::neighbors& neighbors ) {
				neighbors = all_neighbors[index];
			}

			// Distance among points, input as indices
			float get_dist( hvt::index i, hvt::index j ) {
				return dist(i,j);
			}

	};
}