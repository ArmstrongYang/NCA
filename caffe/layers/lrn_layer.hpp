#pragma once
#include <vector>

#include "blob.hpp"
#include "layer.hpp"
#include "proto/caffe.pb.h"

#include "layers/eltwise_layer.hpp"
#include "layers/pooling_layer.hpp"
#include "layers/power_layer.hpp"
#include "layers/split_layer.hpp"

namespace caffe {
/**
* @brief Normalize the input in a local region across or within feature maps.
*
* TODO(dox): thorough documentation for Forward, Backward, and proto params.
*/
template <typename Dtype>
class LRNLayer : public Layer<Dtype> {
public:
	explicit LRNLayer(const LayerParameter& param)
		: Layer<Dtype>(param) {}
	virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);
	virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	virtual inline const char* type() const { return "LRN"; }
	virtual inline int ExactNumBottomBlobs() const { return 1; }
	virtual inline int ExactNumTopBlobs() const { return 1; }

protected:
	virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);


	virtual void CrossChannelForward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	virtual void WithinChannelForward(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);
	

	int size_;
	int pre_pad_;
	Dtype alpha_;
	Dtype beta_;
	Dtype k_;
	int num_;
	int channels_;
	int height_;
	int width_;

	// Fields used for normalization ACROSS_CHANNELS
	// scale_ stores the intermediate summing results
	Blob<Dtype> scale_;

	// Fields used for normalization WITHIN_CHANNEL
	shared_ptr<SplitLayer<Dtype> > split_layer_;
	vector<Blob<Dtype>*> split_top_vec_;
	shared_ptr<PowerLayer<Dtype> > square_layer_;
	Blob<Dtype> square_input_;
	Blob<Dtype> square_output_;
	vector<Blob<Dtype>*> square_bottom_vec_;
	vector<Blob<Dtype>*> square_top_vec_;
	shared_ptr<PoolingLayer<Dtype> > pool_layer_;
	Blob<Dtype> pool_output_;
	vector<Blob<Dtype>*> pool_top_vec_;
	shared_ptr<PowerLayer<Dtype> > power_layer_;
	Blob<Dtype> power_output_;
	vector<Blob<Dtype>*> power_top_vec_;
	shared_ptr<EltwiseLayer<Dtype> > product_layer_;
	Blob<Dtype> product_input_;
	vector<Blob<Dtype>*> product_bottom_vec_;
};

}  // namespace caffe