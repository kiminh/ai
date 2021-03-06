#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include <grpc/support/time.h>

#include "tensorflow/core/example/example.pb.h"
#include "tensorflow_serving/apis/predict.pb.h"
#include "tensorflow_serving/apis/prediction_service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using tensorflow::serving::PredictRequest;
using tensorflow::serving::PredictResponse;
using tensorflow::serving::PredictionService;
using tensorflow::TensorProto;
using tensorflow::TensorShapeProto;
using tensorflow::DataType;

class PredictionClient {
 public:
  PredictionClient(std::shared_ptr<Channel> channel)
      : stub_(PredictionService::NewStub(channel)) {}

  void Predict(const std::string& serve_name) {
    PredictRequest request;
    auto* model_spec = request.mutable_model_spec();
    model_spec->set_name(serve_name);
    model_spec->set_signature_name("predicts");

    // Create Example
    ::tensorflow::Example example;
    auto features = example.mutable_features();
    auto feature = features->mutable_feature();
    ::tensorflow::Feature words;
    ::tensorflow::Feature age;
    ::tensorflow::Feature gender;
    auto bytes_list = words.mutable_bytes_list();
    auto age_float_list = age.mutable_float_list();
    auto gender_int_list = gender.mutable_int64_list();

    const int recieve_ws = 100;
    for (int i = 0; i < recieve_ws; ++i) {
      bytes_list->add_value("");
    }
    bytes_list->set_value(0, "6215a4e92df895aa");
    age_float_list->add_value(7.0);
    gender_int_list->add_value(1);
    (*feature)["words"] = words;
    (*feature)["age"] = age;
    (*feature)["gender"] = gender;

    std::string serialized;
    example.SerializeToString(&serialized);

    // Create input tensor
    TensorProto input_tensor;
    TensorShapeProto shape;
    auto* dim = shape.add_dim();
    dim->set_size(1);

    auto* input_shape = input_tensor.mutable_tensor_shape();
    (*input_shape) = shape;
    input_tensor.set_dtype(DataType::DT_STRING);
    input_tensor.add_string_val(serialized);

    // Set inputs
    auto* inputs = request.mutable_inputs();
    (*inputs)["examples"] = input_tensor;

    PredictResponse response;
    ClientContext context;
    std::cout << "Predict ..." << std::endl;
    gpr_timespec timespec;
    timespec.tv_sec = 0;
    timespec.tv_nsec = 10 * 1000 * 1000;  // ms
    timespec.clock_type = GPR_TIMESPAN;
    context.set_deadline(timespec);
    Status status = stub_->Predict(&context, request, &response);
    if (status.ok()) {
      std::cout << "OK" << std::endl;
      std::cout << response.DebugString() << std::endl;
    } else {
      std::cout << "error: code = " << status.error_code() << ": "
                << status.error_message() << std::endl;
      exit(-1);
    }
    auto& outputs = response.outputs();

    auto& scores = outputs.at("scores");
    auto& rowkeys = outputs.at("words");
    auto& num_in_dict = outputs.at("num_in_dict");

    std::vector<std::pair<float, std::string>> recalled;
    if (num_in_dict.int64_val_size() > 0 && num_in_dict.int64_val(0) > 0) {
      for (int i = 0; i < scores.float_val_size(); ++i) {
        recalled.push_back({scores.float_val(i), rowkeys.string_val(i)});
      }
    }

    std::cout << "Predictions: " << std::endl;
    for (auto& p : recalled) {
      std::cout << p.first << " : " << p.second << std::endl;
    }
  }

 private:
  std::unique_ptr<PredictionService::Stub> stub_;
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: <ip:port>" << std::endl;
    exit(-1);
  }
  PredictionClient client(
      grpc::CreateChannel(argv[1], grpc::InsecureChannelCredentials()));
  client.Predict("knet");

  return 0;
}
