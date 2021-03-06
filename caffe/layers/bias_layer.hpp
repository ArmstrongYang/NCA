#pragma once
#include <vector>

#include "blob.hpp"
#include "layer.hpp"
#include "proto/caffe.pb.h"

namespace caffe {

template <typename Dtype>
class BiasLayer : public Layer<Dtype> {
public:
	explicit BiasLayer(const LayerParameter& param)
		: Layer<Dtype>(param) {}
	virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);
	virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	virtual inline const char* type() const { return "Bias"; }
	virtual inline int MinBottomBlobs() const { return 1; }
	virtual inline int MaxBottomBlobs() const { return 2; }
	virtual inline int ExactNumTopBlobs() const { return 1; }

	virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

private:
	Blob<Dtype> bias_multiplier_;
	int outer_dim_, bias_dim_, inner_dim_, dim_;
};



}  // namespace caffe
