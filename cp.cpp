#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib> 
#include <set>
#include <map>
#include <stdlib.h>
#include <random>
#include <math.h>
#include <cmath>
#include <limits>

class Data{
public:
	std::vector< std::vector <std::string> > triple;
	std::map<std::string, int> entity_map;
	std::map<std::string, int> relation_map;
	std::vector< std::vector < int > > tripleID;
	int entity_num;
	int relation_num;
	int relation_numkari;
	int relation_cnt;
	Data(){}
	std::vector< std::vector <std::string> > load(const std::string& file_name){
		relation_cnt = 0;
        	std::fstream fs;
		std::vector<std::string> tmp(3);
		std::string l,m,n;
		fs.open(file_name, std::ios::in);
		while (fs >> l >> m >> n){
			tmp[0] = l;
			tmp[1] = m;
			tmp[2] = n;
			
			triple.push_back(tmp);
		}
		
		fs.close(); 
		return triple;
	}
	void add_rev(void){
		std::vector<std::string> tmp(3);
		std::string l, m, n;
		int triplesize = triple.size();
		for(int i=0; i<triplesize; i++){
			tmp[0] = triple[i][2];
			tmp[1] = "**" + triple[i][1];
			tmp[2] = triple[i][0];

			triple.push_back(tmp);

		}
	}


	std::map<std::string, int> entitydict(std::vector< std::vector < std::string > >& triple){
		int entity_cnt = 0;
		for(int i=0; i<triple.size(); i++){
			if(entity_map.find(triple[i][0]) == entity_map.end()){
				entity_map[triple[i][0]] = entity_cnt;
				entity_cnt++;
			}
			if(entity_map.find(triple[i][2]) == entity_map.end()){
				entity_map[triple[i][2]] = entity_cnt;
				entity_cnt++;
			}
		}
		entity_num = entity_cnt;
		return entity_map;
	}
	std::map<std::string, int> relationdict(std::vector< std::vector < std::string > >& triple){
		for(int i=0; i<triple.size(); i++){
			if(relation_map.find(triple[i][1]) == relation_map.end()){
				relation_map[triple[i][1]] = relation_cnt;
				relation_cnt++;
			}
		}
		relation_num = relation_cnt;
		return relation_map;
	}

	std::map<std::string, int> rev_dict(){
		int count = 0;
		std::string revkey;
		for(auto itr = relation_map.begin(); itr != relation_map.end(); ++itr) {
			revkey = "**" + itr->first;
			relation_map[revkey] = itr->second + relation_numkari;
    		}
		for(auto itr = relation_map.begin(); itr != relation_map.end(); ++itr) {
			count = count + 1;
    		}
		relation_num = count;
		for(auto itr = relation_map.begin(); itr != relation_map.end(); ++itr) {
        		std::cout << "keyyeeeeee = " << itr->first << ", vallllleeeee = " << itr->second << "\n";    // ????
    		}
		return relation_map;
	}
	std::vector< std::vector < int > > makeid(std::map<std::string, int>& entity_map, std::map<std::string, int>& relation_map, std::vector< std::vector < std::string > >& triple){
		std::vector<int> tmp(3);
		for(int i=0; i<triple.size(); i++){
			if (entity_map.find(triple[i][0]) == entity_map.end() || relation_map.find(triple[i][1]) == relation_map.end() || entity_map.find(triple[i][2]) == entity_map.end()){
				std::cout << "idfadfajsdf;akldf;a" << std::endl;
				continue;
			}else{
    
				tmp[0] = entity_map[triple[i][0]];
				tmp[1] = relation_map[triple[i][1]];
				tmp[2] = entity_map[triple[i][2]];

				tripleID.push_back(tmp);
			}


		}
		return tripleID;
	}

};



class CP{
public:
	std::vector< std::vector <double> > subject;
	std::vector< std::vector <double> > relation;
	std::vector< std::vector <double> > object;
	std::vector< std::vector <double> > normalize_subject;
	std::vector< std::vector <double> > normalize_object;
	std::vector< std::vector <double> > normalize_relation;
	int dim;
	double learning_rate;
	double lam;
	Data data;
	Data testdata;
	Data validdata;
	CP(const std::string& trainname, const std::string& testname, const std::string& validname, const int& dimension, const std::string& testidname, const double& rate, const double& lambda){
		dim= dimension;
		lam = lambda;
		learning_rate = rate;
		data.load(trainname); 
		//data.add_rev();
		data.entitydict(data.triple);
		data.relationdict(data.triple);
		//for(auto x:data.relation_map){
		//	std::cout << "key:" << x.first << "value:" << x.second << std::endl;
		//}
		data.makeid(data.entity_map, data.relation_map, data.triple);
		testdata.load(testname); 
		testdata.makeid(data.entity_map, data.relation_map, testdata.triple);
		validdata.load(validname);
		validdata.makeid(data.entity_map, data.relation_map, validdata.triple);
		write_file(testdata.tripleID, data.tripleID, validdata.tripleID, testidname);

		subject = std::vector<std::vector<double>> (data.entity_num, std::vector<double>(dim));
		object = std::vector<std::vector<double>> (data.entity_num, std::vector<double>(dim));
		relation = std::vector<std::vector<double>> (data.relation_num, std::vector<double>(dim));
		normalize_subject = std::vector<std::vector<double>> (data.entity_num, std::vector<double>(dim));
		normalize_object = std::vector<std::vector<double>> (data.entity_num, std::vector<double>(dim));
		normalize_relation = std::vector<std::vector<double>> (data.relation_num, std::vector<double>(dim));
	}

	void write_file( const std::vector < std::vector < int > >& test_tripleID, const std::vector < std::vector < int > >& train_tripleID, const std::vector < std::vector < int > >& valid_tripleID,  const std::string& filename){
		int test_triple_size = test_tripleID.size();
		int sum_triple_size = test_tripleID.size() + train_tripleID.size() + valid_tripleID.size();
		int train_triple_size = train_tripleID.size();
		int valid_triple_size = valid_tripleID.size();

		std::ofstream fout;
		fout.open(filename, std::ios::out|std::ios::binary|std::ios::trunc);
		fout.write((char * ) &test_triple_size,sizeof(int));
		std::cout << test_triple_size << std::endl;
		fout.write((char * ) &sum_triple_size,sizeof(int));
		std::cout << sum_triple_size << std::endl;
		for(int i=0; i<test_triple_size; i++){
			for(int j=0; j<test_tripleID.front().size(); j++){
				fout.write(( char * ) &test_tripleID[i][j], sizeof( int ) );
			}
		}
		for(int i=0; i<train_triple_size; i++){
			for(int j=0; j<train_tripleID.front().size(); j++){
				fout.write(( char * ) &train_tripleID[i][j], sizeof( int ) );
			}
		}
		for(int i=0; i<valid_triple_size; i++){
			for(int j=0; j<valid_tripleID.front().size(); j++){
				fout.write(( char * ) &valid_tripleID[i][j], sizeof( int ) );
			}
		}
		fout.close();

	}

		
	void randominitialize(const int&entity_size, const int&relation_size){
		std::random_device rnd;     
    		std::mt19937 mt(rnd());     
    		std::uniform_real_distribution<> rand100(-sqrt(6)/sqrt(2*dim), sqrt(6)/sqrt(2*dim));
		for(int i=0;i<entity_size;i++){
			for(int j=0;j<dim;j++){
				subject[i][j] = rand100(mt);
				object[i][j] = rand100(mt);
			}
		}

		for(int i=0;i<relation_size;i++){
			for(int j=0;j<dim;j++){
				relation[i][j] = rand100(mt);
			}
		}

	}

	double scorefuntion(std::vector< double >& vector1, std::vector< double >& vector2, std::vector< double >& vector3){
		double score = 0;
		for (int i=0;i<dim;i++){
			score += vector1[i] * vector2[i] * vector3[i];
		}
		return score;
	}


	double sigmoid(double score){
		double sigmoid;
		sigmoid = 1 / (1 + std::exp(-score));
		return sigmoid;
	}

	std::vector< double > product(std::vector< double > vector1, std::vector< double > vector2){
		std::vector< double > vector(dim);
		for(int i=0; i<dim; i++){
			vector[i] = vector1[i] * vector2[i];
		}
		return vector;
	}

	double innerproduct(std::vector< double > vector1, std::vector<double> vector2){
		double score = 0;
		for(int i=0; i<dim; i++){
			score += vector1[i] * vector2[i];
		}
		return score;
	}
			

	void updater(const int& subject_elem, const int& object_elem, const int& relation_elem, int y){
		std::vector< double >  outvec1(dim);
		std::vector< double >  outvec2(dim);
		std::vector< double >  outvec3(dim);
		std::vector< double >  s_gradient(dim);
		std::vector< double >  o_gradient(dim);
		std::vector< double >  r_gradient(dim);
		double yscore;
		double score = 0;
		double norm_sub;
		double norm_rel;
		double norm_obj;
		double threshold = 5;
		outvec1 = product(object[object_elem], relation[relation_elem]);
		outvec2 = product(subject[subject_elem], relation[relation_elem]);
		outvec3 = product(subject[subject_elem], object[object_elem]);
		score = innerproduct(subject[subject_elem], outvec1);
		yscore = y * score;
		for(int i=0; i<dim; i++){
			s_gradient[i] = - y * sigmoid(-yscore) * outvec1[i];
			o_gradient[i] = - y * sigmoid(-yscore) * outvec2[i];
			r_gradient[i] = - y * sigmoid(-yscore) * outvec3[i];
		} 
		norm_sub = normfunction(s_gradient);
		norm_obj = normfunction(o_gradient);
		norm_rel = normfunction(r_gradient);
		for(int i=0; i<dim; i++){
			if(norm_sub >= threshold){
				double x = threshold / norm_sub;
				subject[subject_elem][i] = subject[subject_elem][i] - x * learning_rate * s_gradient[i];
			}else{
				subject[subject_elem][i] = subject[subject_elem][i] - learning_rate * s_gradient[i];
				
			}
			if(norm_obj >= threshold){
				double x = threshold / norm_obj;
				object[object_elem][i] = object[object_elem][i] - x * learning_rate * o_gradient[i];
			}else{
				object[object_elem][i] = object[object_elem][i] - learning_rate * o_gradient[i];
				
			}
			if(norm_rel >= threshold){
				double x = threshold / norm_rel;
				relation[relation_elem][i] = relation[relation_elem][i] - x * learning_rate * r_gradient[i];
			}else{
				relation[relation_elem][i] = relation[relation_elem][i] - learning_rate * r_gradient[i];
				
			}

		}
		
	}


	double normfunction(std::vector< double >& vector){
		double norm = 0.0;
		for(int i=0; i<vector.size(); i++){
			norm = norm + std::pow(vector[i], 2.0);
		}
		norm = sqrt(norm);
		return norm;
	}


	void write_model(const std::vector < std::vector < double > >& matrix, const std::string& filename){
		int matrix_size = matrix.size();
		std::ofstream fout;
		fout.open(filename, std::ios::out|std::ios::binary|std::ios::trunc);
		fout.write((char * ) &matrix_size,sizeof(int));
		fout.write((char * ) &dim,sizeof(int));
		for(int i=0; i<matrix.size(); i++){
			for(int j=0; j<dim; j++){
				fout.write(( char * ) &matrix[i][j], sizeof( double ) );
			}
		}
		fout.close();
	}
	void normalize_vector(void){
		double norm_subject = 0; 
		double norm_object = 0; 
		double norm_relation = 0; 

		for(int i=0; i<subject.size(); i++){
			norm_subject = normfunction(subject[i]);
			norm_object = normfunction(object[i]);
			for(int j=0; j<dim; j++){
				normalize_subject[i][j] = subject[i][j]  / normfunction(subject[i]);
				normalize_object[i][j] = object[i][j] / normfunction(object[i]);
			}
		}

		for(int i=0; i<relation.size(); i++){
			norm_relation= normfunction(relation[i]);
			for(int j=0; j<dim; j++){
				normalize_relation[i][j] = relation[i][j] / normfunction(relation[i]);
			}
		}

	}


	void train(const int& iter){
		int random_num;
		randominitialize(data.entity_num, data.relation_num);
		std::cout << "finish randominitialize relation" << std::endl;
		int random_sample = 5;
		std::vector< double > a, b, c;
		std::random_device rnd;     
    		std::mt19937 mt(rnd());     
    		std::uniform_int_distribution<> rand(0, data.entity_num-1);
		std::vector<int> v = std::vector<int>(data.tripleID.size());
		for(int i=0; i<data.tripleID.size(); i++){
			v[i] = i;
		}
		for(int i=0; i<iter; i++){
			std::random_device seed_gen;
			std::mt19937 engine(seed_gen());
			std::shuffle(v.begin(), v.end(), engine);
			
			int cnt = 1;
			for (auto j: v){
				int subject_elem = data.tripleID[j][0];
				int object_elem = data.tripleID[j][2];
				int relation_elem = data.tripleID[j][1];
				if(cnt % 1000 == 0){
					std::cout << cnt << ' ' << std::flush;
				}
				cnt++;
				updater(subject_elem, object_elem, relation_elem, 1);

				for(int k=0;k<random_sample;k++){
					random_num = rand(mt);
					updater(random_num, object_elem, relation_elem, -1);
					random_num = rand(mt);
					updater(subject_elem, random_num, relation_elem, -1);
				}
			}
			std::cout << std::endl;
			std::cout<< "---------" << i << "----------" << std::endl;
			std::cout << "scorreeeee:" << scorefuntion(subject[data.tripleID[0][0]], object[data.tripleID[0][2]], relation[data.tripleID[0][1]]) << std::endl;
			std::cout<< "--------------------" << std::endl;
			if(i % 10 ==0){
				std::string subject_name = "./model/" + std::to_string(i) + "_subject.txt";
				std::string object_name = "./model/" + std::to_string(i) + "_object.txt";
				std::string relation_name = "./model/" + std::to_string(i) + "_relation.txt";
				normalize_vector();
				write_model(normalize_subject, subject_name);
				write_model(normalize_object, object_name);
				write_model(normalize_relation, relation_name);
				for(int x=0; x<data.tripleID.size(); x++){
					std::cout << scorefuntion(subject[data.tripleID[x][0]], object[data.tripleID[x][2]], relation[data.tripleID[x][1]]) << std::endl;
				}
			}

		}

	}
};


int ArgPos(std::string str, int argc, char **argv) {
	int a;
	for (a = 1; a < argc; a++){
		if (str == argv[a]) {
			if (a == argc - 1) {
				std::cout << "Argument missing for" << str << std::endl;
				exit(1);
			}
			return a;
		}
	}
	return -1;
}

int main(int argc, char **argv){
	std::string trainname;
	std::string testname;
	std::string validname;
	int dimension;
	int iteration;
	std::string testidname;
	double rate;
	double lambda;
	int i;
	if ((i = ArgPos((char *)"-train", argc, argv)) > 0) trainname = argv[i + 1];
	if ((i = ArgPos((char *)"-test", argc, argv)) > 0) testname = argv[i + 1];
	if ((i = ArgPos((char *)"-valid", argc, argv)) > 0) validname = argv[i + 1];
	if ((i = ArgPos((char *)"-dimension", argc, argv)) > 0) dimension = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-iteration", argc, argv)) > 0) iteration = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-testid", argc, argv)) > 0) testidname = argv[i + 1];
	if ((i = ArgPos((char *)"-rate", argc, argv)) > 0) rate = std::stof(argv[i + 1]);
	if ((i = ArgPos((char *)"-lambda", argc, argv)) > 0) lambda = std::stof(argv[i + 1]);
	CP cp(trainname, testname, validname, dimension, testidname, rate, lambda);
	cp.train(iteration);

	return 0;
}
