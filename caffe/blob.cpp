#include <climits>
#include <vector>

#include "blob.hpp"
#include "common.hpp"
#include "syncedmem.hpp"
//#include "caffe/util/math_functions.hpp"

namespace caffe {

template <typename Dtype>
void Blob<Dtype>::Reshape(const int num, const int channels, const int height,
	const int width) 
{
	vector<int> shape(4);
	shape[0] = num;
	shape[1] = channels;
	shape[2] = height;
	shape[3] = width;
	Reshape(shape);
}

template <typename Dtype>
void Blob<Dtype>::Reshape(const vector<int>& shape) 
{
	CHECK_LE(shape.size(), kMaxBlobAxes);
	count_ = 1;
	shape_.resize(shape.size());
	if (!shape_data_ || shape_data_->size() < shape.size() * sizeof(int)) {
		shape_data_.reset(new SyncedMemory(shape.size() * sizeof(int)));
	}
	int* shape_data = static_cast<int*>(shape_data_->mutable_cpu_data());
	for (int i = 0; i < shape.size(); ++i) {
		CHECK_GE(shape[i], 0);
		if (count_ != 0) {
			CHECK_LE(shape[i], INT_MAX / count_) << "blob size exceeds INT_MAX";
		}
		count_ *= shape[i];
		shape_[i] = shape[i];
		shape_data[i] = shape[i];
	}
	if (count_ > capacity_) {
		capacity_ = count_;
		data_.reset(new SyncedMemory(capacity_ * sizeof(Dtype)));
	}
}

template <typename Dtype>
void Blob<Dtype>::Reshape(const BlobShape& shape) {
	CHECK_LE(shape.dim_size(), kMaxBlobAxes);
	vector<int> shape_vec(shape.dim_size());
	for (int i = 0; i < shape.dim_size(); ++i) {
		shape_vec[i] = shape.dim(i);
	}
	Reshape(shape_vec);
}

template <typename Dtype>
void Blob<Dtype>::ReshapeLike(const Blob<Dtype>& other) {
	Reshape(other.shape());
}

template <typename Dtype>
Blob<Dtype>::Blob(const int num, const int channels, const int height,
	const int width)
	// capacity_ must be initialized before calling Reshape
	: capacity_(0) {
	Reshape(num, channels, height, width);
}

template <typename Dtype>
Blob<Dtype>::Blob(const vector<int>& shape)
	// capacity_ must be initialized before calling Reshape
	: capacity_(0) {
	Reshape(shape);
}

template <typename Dtype>
const Dtype* Blob<Dtype>::cpu_data() const {
	CHECK(data_);
	return (const Dtype*)data_->cpu_data();
}

template <typename Dtype>
void Blob<Dtype>::set_cpu_data(Dtype* data) {
	CHECK(data);
	// Make sure CPU and GPU sizes remain equal
	size_t size = count_ * sizeof(Dtype);
	if (data_->size() != size) {
		data_.reset(new SyncedMemory(size));
	}
	data_->set_cpu_data(data);
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_cpu_data() {
	CHECK(data_);
	return static_cast<Dtype*>(data_->mutable_cpu_data());
}

template <typename Dtype>
void Blob<Dtype>::ShareData(const Blob& other) {
	CHECK_EQ(count_, other.count());
	data_ = other.data();
}

template <typename Dtype>
bool Blob<Dtype>::ShapeEquals(const BlobProto& other) {
	if (other.has_num() || other.has_channels() ||
		other.has_height() || other.has_width()) {
		// Using deprecated 4D Blob dimensions --
		// shape is (num, channels, height, width).
		// Note: we do not use the normal Blob::num(), Blob::channels(), etc.
		// methods as these index from the beginning of the blob shape, where legacy
		// parameter blobs were indexed from the end of the blob shape (e.g., bias
		// Blob shape (1 x 1 x 1 x N), IP layer weight Blob shape (1 x 1 x M x N)).
		return shape_.size() <= 4 &&
			LegacyShape(-4) == other.num() &&
			LegacyShape(-3) == other.channels() &&
			LegacyShape(-2) == other.height() &&
			LegacyShape(-1) == other.width();
	}
	vector<int> other_shape(other.shape().dim_size());
	for (int i = 0; i < other.shape().dim_size(); ++i) {
		other_shape[i] = other.shape().dim(i);
	}
	return shape_ == other_shape;
}

template <typename Dtype>
void Blob<Dtype>::FromProto(const BlobProto& proto, bool reshape) {
	if (reshape) {
		vector<int> shape;
		if (proto.has_num() || proto.has_channels() ||
			proto.has_height() || proto.has_width()) {
			// Using deprecated 4D Blob dimensions --
			// shape is (num, channels, height, width).
			shape.resize(4);
			shape[0] = proto.num();
			shape[1] = proto.channels();
			shape[2] = proto.height();
			shape[3] = proto.width();
		}
		else {
			shape.resize(proto.shape().dim_size());
			for (int i = 0; i < proto.shape().dim_size(); ++i) {
				shape[i] = proto.shape().dim(i);
			}
		}
		Reshape(shape);
	}
	else {
		CHECK(ShapeEquals(proto)) << "shape mismatch (reshape not set)";
	}
	// copy data
	Dtype* data_vec = mutable_cpu_data();
	if (proto.double_data_size() > 0) {
		CHECK_EQ(count_, proto.double_data_size());
		for (int i = 0; i < count_; ++i) {
			data_vec[i] = proto.double_data(i);
		}
	}
	else {
		CHECK_EQ(count_, proto.data_size());
		for (int i = 0; i < count_; ++i) {
			data_vec[i] = proto.data(i);
		}
	}
	if (proto.double_diff_size() > 0) {
		//CHECK_EQ(count_, proto.double_diff_size());
		//Dtype* diff_vec = mutable_cpu_diff();
		//for (int i = 0; i < count_; ++i) {
		//	diff_vec[i] = proto.double_diff(i);
		//}
	}
	else if (proto.diff_size() > 0) {
		//CHECK_EQ(count_, proto.diff_size());
		//Dtype* diff_vec = mutable_cpu_diff();
		//for (int i = 0; i < count_; ++i) {
		//	diff_vec[i] = proto.diff(i);
		//}
	}
}

INSTANTIATE_CLASS(Blob);
template class Blob<int>;
template class Blob<unsigned int>;

}
