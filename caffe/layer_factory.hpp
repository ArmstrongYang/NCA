#pragma once
#include <map>
#include <string>
#include <vector>
#include <functional>

#include "common.hpp"
#include "proto/caffe.pb.h"
#include "layer.hpp"

namespace caffe {

template <typename Dtype>
class Layer;

template <typename Dtype>
class LayerRegistry {
public:
	typedef shared_ptr<Layer<Dtype> >(*Creator)(const LayerParameter&);
	typedef map<string, Creator> CreatorRegistry;

	static CreatorRegistry& Registry() {
		static CreatorRegistry* g_registry_ = new CreatorRegistry();
		return *g_registry_;
	}

	// Adds a creator.
	static void AddCreator(const string& type, Creator creator) {
		CreatorRegistry& registry = Registry();
		CHECK_EQ(registry.count(type), 0)
			<< "Layer type " << type << " already registered.";
		registry[type] = creator;
	}

	// Get a layer using a LayerParameter.
	static shared_ptr<Layer<Dtype> > CreateLayer(const LayerParameter& param) {
		
		//LOG(INFO) << "Creating layer " << param.name();

		const string& type = param.type();
		CreatorRegistry& registry = Registry();
		CHECK_EQ(registry.count(type), 1) << "Unknown layer type: " << type << "\n" 
			<< "(known types: " << LayerTypeListString() << ")";
		return registry[type](param);
	}

	static vector<string> LayerTypeList() {
		CreatorRegistry& registry = Registry();
		vector<string> layer_types;
		for (typename CreatorRegistry::iterator iter = registry.begin();
			iter != registry.end(); ++iter) {
			layer_types.push_back(iter->first);
		}
		return layer_types;
	}

private:
	// Layer registry should never be instantiated - everything is done with its
	// static variables.
	LayerRegistry() {}

	static string LayerTypeListString() {
		vector<string> layer_types = LayerTypeList();
		string layer_types_str;
		for (vector<string>::iterator iter = layer_types.begin();
			iter != layer_types.end(); ++iter) {
			if (iter != layer_types.begin()) {
				layer_types_str += ", ";
			}
			layer_types_str += *iter;
		}
		return layer_types_str;
	}
};



template <typename Dtype>
class LayerRegisterer {
public:
	LayerRegisterer(const string& type,
		shared_ptr<Layer<Dtype> >(*creator)(const LayerParameter&)) {
		// LOG(INFO) << "Registering layer type: " << type;
		LayerRegistry<Dtype>::AddCreator(type, creator);
	}
};


#define REGISTER_LAYER_CREATOR(type, creator)                                  \
  static LayerRegisterer<float> g_creator_f_##type(#type, creator<float>);     

#define REGISTER_LAYER_CLASS(type)                                             \
  template <typename Dtype>                                                    \
  shared_ptr<Layer<Dtype> > Creator_##type##Layer(const LayerParameter& param) \
  {                                                                            \
    return shared_ptr<Layer<Dtype> >(new type##Layer<Dtype>(param));           \
  }                                                                            \
  REGISTER_LAYER_CREATOR(type, Creator_##type##Layer)

}
