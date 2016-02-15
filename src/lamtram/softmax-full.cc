#include <lamtram/softmax-full.h>
#include <lamtram/macros.h>
#include <cnn/expr.h>

using namespace lamtram;
using namespace cnn::expr;

SoftmaxFull::SoftmaxFull(const std::string & sig, int input_size, const VocabularyPtr & vocab, cnn::Model & mod) : SoftmaxBase(sig,input_size,vocab,mod) {
  p_sm_W_ = mod.add_parameters({(unsigned int)vocab->size(), (unsigned int)input_size});
  p_sm_b_ = mod.add_parameters({(unsigned int)vocab->size()});  
}

void SoftmaxFull::NewGraph(cnn::ComputationGraph & cg) {
  i_sm_b_ = parameter(cg, p_sm_b_);
  i_sm_W_ = parameter(cg, p_sm_W_);
}

// Calculate training loss for one word
cnn::expr::Expression SoftmaxFull::CalcLoss(cnn::expr::Expression & in, WordId word, bool train) {
  Expression score = affine_transform({i_sm_b_, i_sm_W_, in});
  return pickneglogsoftmax(score, word);
}
// Calculate training loss for multiple words
cnn::expr::Expression SoftmaxFull::CalcLoss(cnn::expr::Expression & in, const std::vector<WordId> & word, bool train) {
  Expression score = affine_transform({i_sm_b_, i_sm_W_, in});
  std::vector<unsigned> wvec(word.size());
  for(size_t i = 0; i < word.size(); i++)
    wvec[i] = word[i];
  return pickneglogsoftmax(score, wvec);
}

// Calculate the full probability distribution
cnn::expr::Expression SoftmaxFull::CalcProbability(cnn::expr::Expression & in) {
  return softmax(affine_transform({i_sm_b_, i_sm_W_, in}));
}
cnn::expr::Expression SoftmaxFull::CalcLogProbability(cnn::expr::Expression & in) {
  return log_softmax(affine_transform({i_sm_b_, i_sm_W_, in}));
}

