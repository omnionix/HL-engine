// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_SEQUENCE_NULLSPACE_HAPTICFILES_H_
#define FLATBUFFERS_GENERATED_SEQUENCE_NULLSPACE_HAPTICFILES_H_

#include "flatbuffers/flatbuffers.h"

#include "HapticEffect_generated.h"

namespace NullSpace {
namespace HapticFiles {

struct Sequence;

struct Sequence FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_LOCATION = 4,
    VT_ITEMS = 6
  };
  uint32_t location() const { return GetField<uint32_t>(VT_LOCATION, 0); }
  const flatbuffers::Vector<flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>> *items() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>> *>(VT_ITEMS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_LOCATION) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_ITEMS) &&
           verifier.Verify(items()) &&
           verifier.VerifyVectorOfTables(items()) &&
           verifier.EndTable();
  }
};

struct SequenceBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_location(uint32_t location) { fbb_.AddElement<uint32_t>(Sequence::VT_LOCATION, location, 0); }
  void add_items(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>>> items) { fbb_.AddOffset(Sequence::VT_ITEMS, items); }
  SequenceBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  SequenceBuilder &operator=(const SequenceBuilder &);
  flatbuffers::Offset<Sequence> Finish() {
    auto o = flatbuffers::Offset<Sequence>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<Sequence> CreateSequence(flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t location = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>>> items = 0) {
  SequenceBuilder builder_(_fbb);
  builder_.add_items(items);
  builder_.add_location(location);
  return builder_.Finish();
}

inline flatbuffers::Offset<Sequence> CreateSequenceDirect(flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t location = 0,
    const std::vector<flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>> *items = nullptr) {
  return CreateSequence(_fbb, location, items ? _fbb.CreateVector<flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>>(*items) : 0);
}

inline const NullSpace::HapticFiles::Sequence *GetSequence(const void *buf) {
  return flatbuffers::GetRoot<NullSpace::HapticFiles::Sequence>(buf);
}

inline bool VerifySequenceBuffer(flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<NullSpace::HapticFiles::Sequence>(nullptr);
}

inline void FinishSequenceBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<NullSpace::HapticFiles::Sequence> root) {
  fbb.Finish(root);
}

}  // namespace HapticFiles
}  // namespace NullSpace

#endif  // FLATBUFFERS_GENERATED_SEQUENCE_NULLSPACE_HAPTICFILES_H_
