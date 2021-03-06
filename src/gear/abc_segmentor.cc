//
// Copyleft RIME Developers
// License: GPLv3
//
// 2011-05-15 GONG Chen <chen.sst@gmail.com>
//
#include <rime/common.h>
#include <rime/config.h>
#include <rime/schema.h>
#include <rime/segmentation.h>
#include <rime/gear/abc_segmentor.h>

static const char kRimeAlphabet[] = "zyxwvutsrqponmlkjihgfedcba";

namespace rime {

AbcSegmentor::AbcSegmentor(const Ticket& ticket)
    : Segmentor(ticket), alphabet_(kRimeAlphabet) {
  if (!ticket.schema)
    return;
  if (Config* config = ticket.schema->config()) {
    config->GetString("speller/alphabet", &alphabet_);
    config->GetString("speller/delimiter", &delimiter_);
    if (auto extra_tags = config->GetList("abc_segmentor/extra_tags")) {
      for (size_t i = 0; i < extra_tags->size(); ++i) {
        if (auto value = As<ConfigValue>(extra_tags->GetAt(i))) {
          extra_tags_.insert(value->str());
        }
      }
    }
  }
}

bool AbcSegmentor::Proceed(Segmentation* segmentation) {
  const std::string& input(segmentation->input());
  DLOG(INFO) << "abc_segmentor: " << input;
  size_t j = segmentation->GetCurrentStartPosition();
  size_t k = j;
  for (; k < input.length(); ++k) {
    bool is_letter = alphabet_.find(input[k]) != std::string::npos;
    bool is_delimiter =
        (k != j) && (delimiter_.find(input[k]) != std::string::npos);
    if (!is_letter && !is_delimiter)
      break;
  }
  DLOG(INFO) << "[" << j << ", " << k << ")";
  if (j < k) {
    Segment segment(j, k);
    segment.tags.insert("abc");
    for (const std::string& tag : extra_tags_) {
      segment.tags.insert(tag);
    }
    segmentation->AddSegment(segment);
  }
  // continue this round
  return true;
}

}  // namespace rime
