#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/gtl/map_util.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/random/distribution_sampler.h"
#include "tensorflow/core/lib/random/philox_random.h"
#include "tensorflow/core/lib/random/simple_philox.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/platform/default/integral_types.h"
#include "tensorflow/core/platform/posix/posix_file_system.h"
#include "tensorflow/core/platform/thread_annotations.h"
#include "tensorflow/core/util/guarded_philox_random.h"

#include <time.h>

#include <fstream>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "args.h"
#include "defines.h"
#include "dictionary.h"

namespace tensorflow {

class FasttextExampleGenerateOp : public OpKernel {
 public:
  explicit FasttextExampleGenerateOp(OpKernelConstruction* ctx)
      : OpKernel(ctx), global_lines_(0) {
    LOG(INFO) << "Init FasttextExampleGenerateOp ...";
    args_ = std::make_shared<::fasttext::Args>();
    ParseArgs(ctx);
    rng_.seed(time(NULL));

    dict_ = std::make_shared<::fasttext::Dictionary>(args_);

    if (!args_->use_saved_dict) {
      PreProcessTrainData(ctx);
      SaveDictionary(ctx);
    } else {
      LoadDictionary(ctx);
    }

    LOG(ERROR) << "nwords = " << dict_->nwords();
    LOG(ERROR) << "nlabels = " << dict_->nlabels();

    LOG(INFO) << "Init FasttextExampleGenerateOp OK";
  }

  void Compute(OpKernelContext* ctx) override {
    if (!args_->use_saved_dict) {
      TensorShape shape;
      Tensor* records_tensor = NULL;
      OP_REQUIRES_OK(ctx, ctx->allocate_output(0, shape, &records_tensor));
      Tensor* labels_tensor = NULL;
      OP_REQUIRES_OK(ctx, ctx->allocate_output(1, shape, &labels_tensor));

      return;
    }
    ++global_lines_;
    auto x = global_lines_.load(std::memory_order_relaxed);
    if (x % 10000 == 0) {
      LOG(ERROR) << "global lines = " << x;
    }

    const Tensor& input_tensor = ctx->input(0);
    auto flat_input = input_tensor.flat<std::string>();

    std::vector<int32_t> words;
    std::vector<std::vector<int>> insts;
    for (int i = 0; i < flat_input.size(); ++i) {
      words.clear();
      std::stringstream ss(flat_input(i));
      int ntokens = dict_->getLine(ss, words, rng_);
      std::vector<int> bow;
      std::uniform_int_distribution<> uniform(1, args_->ws);
      std::uniform_real_distribution<> dropout_uniform(0, 1);
      // genearte examples
      for (int w = 1; w < words.size(); w++) {
        if (dropout_uniform(rng_) < args_->sample_dropout) {
          continue;
        }

        // use words[w] as the first label
        int32_t boundary = std::min(w, uniform(rng_));
        bow.clear();
        for (int c = -boundary; c < 0; c++) {
          bow.push_back(words[w + c]);
        }
        bow.push_back(words[w]);  // add label

        // TODO random select ntargets-1 labels
        for (int i = 0; i < args_->ntargets - 1; ++i) {
          int t = w + 1 + i;
          if (t >= words.size()) {
            t = w;
          }
          bow.push_back(words[t]);
        }

        insts.push_back(bow);
      }
    }

    // Create output tensors
    TensorShape records_shape;
    records_shape.AddDim(insts.size());
    records_shape.AddDim(args_->ws);
    Tensor* records_tensor = NULL;
    OP_REQUIRES_OK(ctx,
                   ctx->allocate_output(0, records_shape, &records_tensor));

    TensorShape labels_shape;
    labels_shape.AddDim(insts.size());
    labels_shape.AddDim(args_->ntargets);
    Tensor* labels_tensor = NULL;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(1, labels_shape, &labels_tensor));

    auto matrix_records = records_tensor->matrix<int32>();
    auto matrix_labels = labels_tensor->matrix<int64>();
    for (int inst_index = 0; inst_index < insts.size(); ++inst_index) {
      auto& inst = insts[inst_index];
      OP_REQUIRES(ctx, inst.size() >= args_->ntargets,
                  errors::InvalidArgument(
                      "inst size should larger or equal than ntargets"));
      // fill labels
      for (int t = 0; t < args_->ntargets; ++t) {
        int index = inst.size() - args_->ntargets + t;
        matrix_labels(inst_index, t) = transform_id(inst[index]);
      }

      // fill records
      for (int i = 0; i < inst.size() - args_->ntargets; ++i) {
        matrix_records(inst_index, i) = transform_id(inst[i]);
      }

      // padding records
      for (int i = inst.size() - args_->ntargets; i < args_->ws; ++i) {
        matrix_records(inst_index, i) = PADDING_INDEX;
      }
    }
  }

 private:
  void ParseArgs(OpKernelConstruction* ctx) {
    OP_REQUIRES_OK(ctx,
                   ctx->GetAttr("train_data_path", &args_->train_data_path));
    LOG(INFO) << "train_data_path: " << args_->train_data_path;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("ws", &args_->ws));
    LOG(INFO) << "ws: " << args_->ws;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("min_count", &args_->min_count));
    LOG(INFO) << "min_count: " << args_->min_count;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("t", &args_->t));
    LOG(INFO) << "t: " << args_->t;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("verbose", &args_->verbose));
    LOG(INFO) << "verbose: " << args_->verbose;

    OP_REQUIRES_OK(ctx,
                   ctx->GetAttr("min_count_label", &args_->min_count_label));
    LOG(INFO) << "min_count_label: " << args_->min_count_label;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("label", &args_->label));
    LOG(INFO) << "label: " << args_->label;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("use_saved_dict", &args_->use_saved_dict));
    LOG(INFO) << "use_saved_dict: " << args_->use_saved_dict;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("dict_dir", &args_->dict_dir));
    LOG(INFO) << "dict_dir: " << args_->dict_dir;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("ntargets", &args_->ntargets));
    LOG(INFO) << "ntargets: " << args_->ntargets;

    OP_REQUIRES_OK(ctx, ctx->GetAttr("sample_dropout", &args_->sample_dropout));
    LOG(INFO) << "sample_dropout: " << args_->sample_dropout;
  }

  void PreProcessTrainData(OpKernelConstruction* ctx) {
    LOG(INFO) << "Preprocess train data beginning ...";
    std::ifstream ifs(args_->train_data_path);
    OP_REQUIRES(ctx, ifs.is_open(),
                errors::Unavailable(args_->train_data_path + " open failed."));
    dict_->readFromFile(ifs);
    ifs.close();
    LOG(INFO) << "Preprocess train data done.";
  }

  inline int transform_id(int id) { return id + 1; }

  void SaveDictionary(OpKernelConstruction* ctx) {
    auto root_dir = args_->dict_dir;

    auto file_system = ::tensorflow::PosixFileSystem();
    if (file_system.FileExists(root_dir) != Status::OK()) {
      auto status = file_system.CreateDir(root_dir);
      OP_REQUIRES(ctx, status == Status::OK(),
                  errors::Unavailable("Create dir failed."));
    }

    LOG(INFO) << "SaveDictionary to " << root_dir << " ...";
    auto saved_dict = ::tensorflow::io::JoinPath(root_dir, SAVED_DICT);
    auto dict_meta = ::tensorflow::io::JoinPath(root_dir, DICT_META);
    auto dict_words = ::tensorflow::io::JoinPath(root_dir, DICT_WORDS);

    {
      // save dictionary
      LOG(INFO) << "Save dictionary to " << saved_dict << " ...";
      std::ofstream ofs(saved_dict);
      OP_REQUIRES(ctx, ofs.is_open(),
                  errors::Unavailable(saved_dict + " open failed"));
      dict_->save(ofs);
      OP_REQUIRES(ctx, ofs.good(), errors::Unavailable("Write error"));
      ofs.close();
      LOG(INFO) << "Save dictionary OK";
    }

    {
      // save dict meta
      std::ofstream ofs(dict_meta);
      LOG(INFO) << "Write dict meta to " << dict_meta << " ...";
      OP_REQUIRES(ctx, ofs.is_open(),
                  errors::Unavailable(dict_meta + " open failed"));
      int nwords = dict_->nwords();
      int nlabels = dict_->nlabels();
      auto to_write = std::string("nwords\t") + std::to_string(nwords) + "\n";
      ofs.write(to_write.data(), to_write.size());

      to_write = std::string("nlabels\t" + std::to_string(nlabels) + "\n");
      ofs.write(to_write.data(), to_write.size());

      OP_REQUIRES(ctx, ofs.good(), errors::Unavailable("Write error!"));
      ofs.close();
      LOG(INFO) << "Write dict meta OK";
    }

    {
      // save dict words
      std::ofstream ofs(dict_words);
      LOG(INFO) << "Write dict words to " << dict_words << " ...";
      OP_REQUIRES(ctx, ofs.is_open(),
                  errors::Unavailable(dict_words + " open failed"));
      for (const auto& entry : dict_->words()) {
        if (entry.type == ::fasttext::entry_type::word) {
          ofs.write(entry.word.data(), entry.word.size());
          ofs.write("\n", 1);
        }
      }
      OP_REQUIRES(ctx, ofs.good(), errors::Unavailable("Write error!"));
      ofs.close();
      LOG(INFO) << "Write dict words OK";
    }
  }

  void LoadDictionary(OpKernelConstruction* ctx) {
    // load dictionary
    auto root_dir = args_->dict_dir;
    auto saved_dict = ::tensorflow::io::JoinPath(root_dir, SAVED_DICT);
    LOG(INFO) << "Load dictionary from " << saved_dict << " ...";
    std::ifstream ifs(saved_dict);
    OP_REQUIRES(ctx, ifs.is_open(),
                errors::Unavailable(saved_dict + " open failed"));
    dict_->load(ifs);
    OP_REQUIRES(ctx, !ifs.fail(), errors::Unavailable("Read error!"));
    ifs.close();
    LOG(INFO) << "Load dictionary OK";
  }

  std::shared_ptr<::fasttext::Args> args_;
  std::shared_ptr<::fasttext::Dictionary> dict_;

  std::minstd_rand rng_;
  std::atomic<long long> global_lines_;
};
REGISTER_KERNEL_BUILDER(Name("FasttextExampleGenerate").Device(DEVICE_CPU),
                        FasttextExampleGenerateOp);

}  // namespace tensorflow
