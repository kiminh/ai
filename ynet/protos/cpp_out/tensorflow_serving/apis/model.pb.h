// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: tensorflow_serving/apis/model.proto

#ifndef PROTOBUF_tensorflow_5fserving_2fapis_2fmodel_2eproto__INCLUDED
#define PROTOBUF_tensorflow_5fserving_2fapis_2fmodel_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/wrappers.pb.h>
// @@protoc_insertion_point(includes)
namespace tensorflow {
namespace serving {
class ModelSpec;
class ModelSpecDefaultTypeInternal;
extern ModelSpecDefaultTypeInternal _ModelSpec_default_instance_;
}  // namespace serving
}  // namespace tensorflow

namespace tensorflow {
namespace serving {

namespace protobuf_tensorflow_5fserving_2fapis_2fmodel_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static void InitDefaultsImpl();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_tensorflow_5fserving_2fapis_2fmodel_2eproto

// ===================================================================

class ModelSpec : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tensorflow.serving.ModelSpec) */ {
 public:
  ModelSpec();
  virtual ~ModelSpec();

  ModelSpec(const ModelSpec& from);

  inline ModelSpec& operator=(const ModelSpec& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ModelSpec(ModelSpec&& from) noexcept
    : ModelSpec() {
    *this = ::std::move(from);
  }

  inline ModelSpec& operator=(ModelSpec&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline ::google::protobuf::Arena* GetArena() const PROTOBUF_FINAL {
    return GetArenaNoVirtual();
  }
  inline void* GetMaybeArenaPointer() const PROTOBUF_FINAL {
    return MaybeArenaPtr();
  }
  static const ::google::protobuf::Descriptor* descriptor();
  static const ModelSpec& default_instance();

  static inline const ModelSpec* internal_default_instance() {
    return reinterpret_cast<const ModelSpec*>(
               &_ModelSpec_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void UnsafeArenaSwap(ModelSpec* other);
  void Swap(ModelSpec* other);
  friend void swap(ModelSpec& a, ModelSpec& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ModelSpec* New() const PROTOBUF_FINAL { return New(NULL); }

  ModelSpec* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const ModelSpec& from);
  void MergeFrom(const ModelSpec& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(ModelSpec* other);
  protected:
  explicit ModelSpec(::google::protobuf::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::google::protobuf::Arena* arena);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);
  ::std::string* unsafe_arena_release_name();
  void unsafe_arena_set_allocated_name(
      ::std::string* name);

  // string signature_name = 3;
  void clear_signature_name();
  static const int kSignatureNameFieldNumber = 3;
  const ::std::string& signature_name() const;
  void set_signature_name(const ::std::string& value);
  #if LANG_CXX11
  void set_signature_name(::std::string&& value);
  #endif
  void set_signature_name(const char* value);
  void set_signature_name(const char* value, size_t size);
  ::std::string* mutable_signature_name();
  ::std::string* release_signature_name();
  void set_allocated_signature_name(::std::string* signature_name);
  ::std::string* unsafe_arena_release_signature_name();
  void unsafe_arena_set_allocated_signature_name(
      ::std::string* signature_name);

  // .google.protobuf.Int64Value version = 2;
  bool has_version() const;
  void clear_version();
  static const int kVersionFieldNumber = 2;
  private:
  void _slow_mutable_version();
  void _slow_set_allocated_version(
      ::google::protobuf::Arena* message_arena, ::google::protobuf::Int64Value** version);
  ::google::protobuf::Int64Value* _slow_release_version();
  public:
  const ::google::protobuf::Int64Value& version() const;
  ::google::protobuf::Int64Value* mutable_version();
  ::google::protobuf::Int64Value* release_version();
  void set_allocated_version(::google::protobuf::Int64Value* version);
  ::google::protobuf::Int64Value* unsafe_arena_release_version();
  void unsafe_arena_set_allocated_version(
      ::google::protobuf::Int64Value* version);

  // @@protoc_insertion_point(class_scope:tensorflow.serving.ModelSpec)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  template <typename T> friend class ::google::protobuf::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::internal::ArenaStringPtr signature_name_;
  ::google::protobuf::Int64Value* version_;
  mutable int _cached_size_;
  friend struct protobuf_tensorflow_5fserving_2fapis_2fmodel_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ModelSpec

// string name = 1;
inline void ModelSpec::clear_name() {
  name_.ClearToEmpty(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), GetArenaNoVirtual());
}
inline const ::std::string& ModelSpec::name() const {
  // @@protoc_insertion_point(field_get:tensorflow.serving.ModelSpec.name)
  return name_.Get();
}
inline void ModelSpec::set_name(const ::std::string& value) {
  
  name_.Set(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value, GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set:tensorflow.serving.ModelSpec.name)
}
#if LANG_CXX11
inline void ModelSpec::set_name(::std::string&& value) {
  
  name_.Set(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_rvalue:tensorflow.serving.ModelSpec.name)
}
#endif
inline void ModelSpec::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.Set(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_char:tensorflow.serving.ModelSpec.name)
}
inline void ModelSpec::set_name(const char* value,
    size_t size) {
  
  name_.Set(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_pointer:tensorflow.serving.ModelSpec.name)
}
inline ::std::string* ModelSpec::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:tensorflow.serving.ModelSpec.name)
  return name_.Mutable(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), GetArenaNoVirtual());
}
inline ::std::string* ModelSpec::release_name() {
  // @@protoc_insertion_point(field_release:tensorflow.serving.ModelSpec.name)
  
  return name_.Release(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), GetArenaNoVirtual());
}
inline ::std::string* ModelSpec::unsafe_arena_release_name() {
  // @@protoc_insertion_point(field_unsafe_arena_release:tensorflow.serving.ModelSpec.name)
  GOOGLE_DCHECK(GetArenaNoVirtual() != NULL);
  
  return name_.UnsafeArenaRelease(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      GetArenaNoVirtual());
}
inline void ModelSpec::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocated(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name,
      GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_allocated:tensorflow.serving.ModelSpec.name)
}
inline void ModelSpec::unsafe_arena_set_allocated_name(
    ::std::string* name) {
  GOOGLE_DCHECK(GetArenaNoVirtual() != NULL);
  if (name != NULL) {
    
  } else {
    
  }
  name_.UnsafeArenaSetAllocated(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      name, GetArenaNoVirtual());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:tensorflow.serving.ModelSpec.name)
}

// .google.protobuf.Int64Value version = 2;
inline bool ModelSpec::has_version() const {
  return this != internal_default_instance() && version_ != NULL;
}
inline void ModelSpec::clear_version() {
  if (GetArenaNoVirtual() == NULL && version_ != NULL) delete version_;
  version_ = NULL;
}
inline const ::google::protobuf::Int64Value& ModelSpec::version() const {
  const ::google::protobuf::Int64Value* p = version_;
  // @@protoc_insertion_point(field_get:tensorflow.serving.ModelSpec.version)
  return p != NULL ? *p : *reinterpret_cast<const ::google::protobuf::Int64Value*>(
      &::google::protobuf::_Int64Value_default_instance_);
}
inline ::google::protobuf::Int64Value* ModelSpec::mutable_version() {
  
  if (version_ == NULL) {
    _slow_mutable_version();
  }
  // @@protoc_insertion_point(field_mutable:tensorflow.serving.ModelSpec.version)
  return version_;
}
inline ::google::protobuf::Int64Value* ModelSpec::release_version() {
  // @@protoc_insertion_point(field_release:tensorflow.serving.ModelSpec.version)
  
  if (GetArenaNoVirtual() != NULL) {
    return _slow_release_version();
  } else {
    ::google::protobuf::Int64Value* temp = version_;
    version_ = NULL;
    return temp;
  }
}
inline  void ModelSpec::set_allocated_version(::google::protobuf::Int64Value* version) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete version_;
  }
  if (version != NULL) {
    _slow_set_allocated_version(message_arena, &version);
  }
  version_ = version;
  if (version) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:tensorflow.serving.ModelSpec.version)
}

// string signature_name = 3;
inline void ModelSpec::clear_signature_name() {
  signature_name_.ClearToEmpty(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), GetArenaNoVirtual());
}
inline const ::std::string& ModelSpec::signature_name() const {
  // @@protoc_insertion_point(field_get:tensorflow.serving.ModelSpec.signature_name)
  return signature_name_.Get();
}
inline void ModelSpec::set_signature_name(const ::std::string& value) {
  
  signature_name_.Set(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value, GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set:tensorflow.serving.ModelSpec.signature_name)
}
#if LANG_CXX11
inline void ModelSpec::set_signature_name(::std::string&& value) {
  
  signature_name_.Set(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_rvalue:tensorflow.serving.ModelSpec.signature_name)
}
#endif
inline void ModelSpec::set_signature_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  signature_name_.Set(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_char:tensorflow.serving.ModelSpec.signature_name)
}
inline void ModelSpec::set_signature_name(const char* value,
    size_t size) {
  
  signature_name_.Set(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_pointer:tensorflow.serving.ModelSpec.signature_name)
}
inline ::std::string* ModelSpec::mutable_signature_name() {
  
  // @@protoc_insertion_point(field_mutable:tensorflow.serving.ModelSpec.signature_name)
  return signature_name_.Mutable(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), GetArenaNoVirtual());
}
inline ::std::string* ModelSpec::release_signature_name() {
  // @@protoc_insertion_point(field_release:tensorflow.serving.ModelSpec.signature_name)
  
  return signature_name_.Release(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), GetArenaNoVirtual());
}
inline ::std::string* ModelSpec::unsafe_arena_release_signature_name() {
  // @@protoc_insertion_point(field_unsafe_arena_release:tensorflow.serving.ModelSpec.signature_name)
  GOOGLE_DCHECK(GetArenaNoVirtual() != NULL);
  
  return signature_name_.UnsafeArenaRelease(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      GetArenaNoVirtual());
}
inline void ModelSpec::set_allocated_signature_name(::std::string* signature_name) {
  if (signature_name != NULL) {
    
  } else {
    
  }
  signature_name_.SetAllocated(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), signature_name,
      GetArenaNoVirtual());
  // @@protoc_insertion_point(field_set_allocated:tensorflow.serving.ModelSpec.signature_name)
}
inline void ModelSpec::unsafe_arena_set_allocated_signature_name(
    ::std::string* signature_name) {
  GOOGLE_DCHECK(GetArenaNoVirtual() != NULL);
  if (signature_name != NULL) {
    
  } else {
    
  }
  signature_name_.UnsafeArenaSetAllocated(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      signature_name, GetArenaNoVirtual());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:tensorflow.serving.ModelSpec.signature_name)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


}  // namespace serving
}  // namespace tensorflow

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_tensorflow_5fserving_2fapis_2fmodel_2eproto__INCLUDED
