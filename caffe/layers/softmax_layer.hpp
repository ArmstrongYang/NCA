#pragma once
#include <vector>

#include "blob.hpp"
#include "layer.hpp"
#include "proto/caffe.pb.h"

namespace caffe {

template <typename Dtype>
class SoftmaxLayer : public Layer<Dtype> {
public:
	explicit SoftmaxLayer(const LayerParameter& param)
		: Layer<Dtype>(param) {}
	virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	virtual inline const char* type() const { return "Softmax"; }
	virtual inline int ExactNumBottomBlobs() const { return 1; }
	virtual inline int ExactNumTopBlobs() const { return 1; }

protected:
	virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top);

	int outer_num_;
	int inner_num_;
	int softmax_axis_;
	/// sum_multiplier is used to carry out sum using BLAS
	Blob<Dtype> sum_multiplier_;
	/// scale is an intermediate Blob to hold temporary results.
	Blob<Dtype> scale_;
};

}  // namespace caffe