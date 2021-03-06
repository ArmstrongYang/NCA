#include <vector>

#include "layers/dropout_layer.hpp"
#include "util/math_functions.hpp"

namespace caffe {

template <typename Dtype>
void DropoutLayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*>& bottom,
	const vector<Blob<Dtype>*>& top) {
	NeuronLayer<Dtype>::LayerSetUp(bottom, top);
	threshold_ = this->layer_param_.dropout_param().dropout_ratio();
	DCHECK(threshold_ > 0.);
	DCHECK(threshold_ < 1.);
	scale_ = 1. / (1. - threshold_);
	uint_thres_ = static_cast<unsigned int>(UINT_MAX * threshold_);
}

template <typename Dtype>
void DropoutLayer<Dtype>::Reshape(const vector<Blob<Dtype>*>& bottom,
	const vector<Blob<Dtype>*>& top) {
	NeuronLayer<Dtype>::Reshape(bottom, top);
	// Set up the cache for random number generation
	// ReshapeLike does not work because rand_vec_ is of Dtype uint
	rand_vec_.Reshape(bottom[0]->shape());
}

template <typename Dtype>
void DropoutLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
	const vector<Blob<Dtype>*>& top) {
	const Dtype* bottom_data = bottom[0]->cpu_data();
	Dtype* top_data = top[0]->mutable_cpu_data();
	unsigned int* mask = rand_vec_.mutable_cpu_data();
	const int count = bottom[0]->count();
	//if (this->phase_ == TRAIN) {
	//	// Create random numbers
	//	caffe_rng_bernoulli(count, 1. - threshold_, mask);
	//	for (int i = 0; i < count; ++i) {
	//		top_data[i] = bottom_data[i] * mask[i] * scale_;
	//	}
	//}
	//else {
		caffe_copy(bottom[0]->count(), bottom_data, top_data);
	//}
}

INSTANTIATE_CLASS(DropoutLayer);
REGISTER_LAYER_CLASS(Dropout);

}  // namespace caffe