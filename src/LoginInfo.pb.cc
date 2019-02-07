// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: LoginInfo.proto

#include "LoginInfo.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)

namespace LoginInfo {
class CredentialDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<Credential>
      _instance;
} _Credential_default_instance_;
}  // namespace LoginInfo
namespace protobuf_LoginInfo_2eproto {
static void InitDefaultsCredential() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::LoginInfo::_Credential_default_instance_;
    new (ptr) ::LoginInfo::Credential();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::LoginInfo::Credential::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_Credential =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsCredential}, {}};

void InitDefaults() {
  ::google::protobuf::internal::InitSCC(&scc_info_Credential.base);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::LoginInfo::Credential, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::LoginInfo::Credential, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::LoginInfo::Credential, username_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::LoginInfo::Credential, password_),
  0,
  1,
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 7, sizeof(::LoginInfo::Credential)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::LoginInfo::_Credential_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "LoginInfo.proto", schemas, file_default_instances, TableStruct::offsets,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\017LoginInfo.proto\022\tLoginInfo\"0\n\nCredenti"
      "al\022\020\n\010username\030\001 \002(\t\022\020\n\010password\030\002 \002(\t"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 78);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "LoginInfo.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_LoginInfo_2eproto
namespace LoginInfo {

// ===================================================================

void Credential::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Credential::kUsernameFieldNumber;
const int Credential::kPasswordFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Credential::Credential()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  ::google::protobuf::internal::InitSCC(
      &protobuf_LoginInfo_2eproto::scc_info_Credential.base);
  SharedCtor();
  // @@protoc_insertion_point(constructor:LoginInfo.Credential)
}
Credential::Credential(const Credential& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  username_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_username()) {
    username_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.username_);
  }
  password_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_password()) {
    password_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.password_);
  }
  // @@protoc_insertion_point(copy_constructor:LoginInfo.Credential)
}

void Credential::SharedCtor() {
  username_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  password_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

Credential::~Credential() {
  // @@protoc_insertion_point(destructor:LoginInfo.Credential)
  SharedDtor();
}

void Credential::SharedDtor() {
  username_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  password_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void Credential::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ::google::protobuf::Descriptor* Credential::descriptor() {
  ::protobuf_LoginInfo_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_LoginInfo_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const Credential& Credential::default_instance() {
  ::google::protobuf::internal::InitSCC(&protobuf_LoginInfo_2eproto::scc_info_Credential.base);
  return *internal_default_instance();
}


void Credential::Clear() {
// @@protoc_insertion_point(message_clear_start:LoginInfo.Credential)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 3u) {
    if (cached_has_bits & 0x00000001u) {
      username_.ClearNonDefaultToEmptyNoArena();
    }
    if (cached_has_bits & 0x00000002u) {
      password_.ClearNonDefaultToEmptyNoArena();
    }
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

bool Credential::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:LoginInfo.Credential)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string username = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_username()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->username().data(), static_cast<int>(this->username().length()),
            ::google::protobuf::internal::WireFormat::PARSE,
            "LoginInfo.Credential.username");
        } else {
          goto handle_unusual;
        }
        break;
      }

      // required string password = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_password()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->password().data(), static_cast<int>(this->password().length()),
            ::google::protobuf::internal::WireFormat::PARSE,
            "LoginInfo.Credential.password");
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:LoginInfo.Credential)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:LoginInfo.Credential)
  return false;
#undef DO_
}

void Credential::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:LoginInfo.Credential)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required string username = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->username().data(), static_cast<int>(this->username().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "LoginInfo.Credential.username");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->username(), output);
  }

  // required string password = 2;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->password().data(), static_cast<int>(this->password().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "LoginInfo.Credential.password");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->password(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:LoginInfo.Credential)
}

::google::protobuf::uint8* Credential::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:LoginInfo.Credential)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required string username = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->username().data(), static_cast<int>(this->username().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "LoginInfo.Credential.username");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->username(), target);
  }

  // required string password = 2;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->password().data(), static_cast<int>(this->password().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "LoginInfo.Credential.password");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->password(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:LoginInfo.Credential)
  return target;
}

size_t Credential::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:LoginInfo.Credential)
  size_t total_size = 0;

  if (has_username()) {
    // required string username = 1;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->username());
  }

  if (has_password()) {
    // required string password = 2;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->password());
  }

  return total_size;
}
size_t Credential::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:LoginInfo.Credential)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  if (((_has_bits_[0] & 0x00000003) ^ 0x00000003) == 0) {  // All required fields are present.
    // required string username = 1;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->username());

    // required string password = 2;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->password());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Credential::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:LoginInfo.Credential)
  GOOGLE_DCHECK_NE(&from, this);
  const Credential* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const Credential>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:LoginInfo.Credential)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:LoginInfo.Credential)
    MergeFrom(*source);
  }
}

void Credential::MergeFrom(const Credential& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:LoginInfo.Credential)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 3u) {
    if (cached_has_bits & 0x00000001u) {
      set_has_username();
      username_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.username_);
    }
    if (cached_has_bits & 0x00000002u) {
      set_has_password();
      password_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.password_);
    }
  }
}

void Credential::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:LoginInfo.Credential)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Credential::CopyFrom(const Credential& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:LoginInfo.Credential)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Credential::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;
  return true;
}

void Credential::Swap(Credential* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Credential::InternalSwap(Credential* other) {
  using std::swap;
  username_.Swap(&other->username_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  password_.Swap(&other->password_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::google::protobuf::Metadata Credential::GetMetadata() const {
  protobuf_LoginInfo_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_LoginInfo_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace LoginInfo
namespace google {
namespace protobuf {
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::LoginInfo::Credential* Arena::CreateMaybeMessage< ::LoginInfo::Credential >(Arena* arena) {
  return Arena::CreateInternal< ::LoginInfo::Credential >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
