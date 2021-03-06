#pragma once
#include <vector>

#include "blob.hpp"
#include "layer.hpp"
#include "proto/caffe.pb.h"

namespace caffe {

template <typename Dtype>
class PoolingLayer : public Layer<Dtype> {
public:
	explicit PoolingLayer(const LayerParameter& param)
		: Layer<Dtype>(param) {}
	virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);
	virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	virtual inline const char* type() const { return "Pooling"; }
	virtual inline int ExactNumBottomBlobs() const { return 1; }
	virtual inline int MinTopBlobs() const { return 1; }
	// MAX POOL layers can output an extra top blob for the mask;
	// others can only output the pooled inputs.
	virtual inline int MaxTopBlobs() const {
		return (this->layer_param_.pooling_param().pool() ==
			PoolingParameter_PoolMethod_MAX) ? 2 : 1;
	}

protected:
	virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	int kernel_h_, kernel_w_;
	int stride_h_, stride_w_;
	int pad_h_, pad_w_;
	int channels_;
	int height_, width_;
	int pooled_height_, pooled_width_;
	bool global_pooling_;
	Blob<Dtype> rand_idx_;
	Blob<int> max_idx_;
};

}  // namespace caffe