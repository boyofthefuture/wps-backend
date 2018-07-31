#include <eosio.wps/proposal.hpp>
#include <eosio.wps/committee.hpp>

namespace eosiowps {

	// @abi action
	void wps_contract::commitvote(account_name watchman, uint64_t proposal_id) {
		require_auth(watchman);

		committee_table committees(_self, _self);
		auto itr = committees.find(watchman);
		eosio_assert(itr != committees.end(), "Account not found in committees table");
		eosio_assert((*itr).is_oversight, "account does not have oversight privileges");

		proposal_table proposals(_self, _self);
		auto idx_index = proposals.get_index<N(idx)>();
		auto itr_proposal = idx_index.find(proposal_id);
		eosio_assert((*itr).is_oversight, "account does not have oversight privileges");
		eosio_assert(itr_proposal != idx_index.end(), "Proposal not found in proposal table");
		eosio_assert((*itr_proposal).status == proposal_status::CHECK_COUNT_VOTES, "Proposal's status is not CHECK_COUNT_VOTES");

		idx_index.modify(itr_proposal, (*itr_proposal).owner, [&](auto& proposal){
			proposal.status = proposal_status::CHECKED_COUNT_VOTES;
		});
	}//action permission should be linked to a separate key

	// @abi action
	void wps_contract::rollbackvote(account_name watchman, uint64_t proposal_id){
		require_auth(watchman);

		committee_table committees(_self, _self);
		auto itr = committees.find(watchman);
		eosio_assert(itr != committees.end(), "Account not found in committees table");
		eosio_assert((*itr).is_oversight, "account does not have oversight privileges");

		proposal_table proposals(_self, _self);
		auto idx_index = proposals.get_index<N(idx)>();
		auto itr_proposal = idx_index.find(proposal_id);
		eosio_assert(itr_proposal != idx_index.end(), "Proposal not found in proposal table");
		eosio_assert((*itr_proposal).status == proposal_status::CHECK_COUNT_VOTES, "Proposal's status is not CHECK_COUNT_VOTES");

		idx_index.modify(itr_proposal, (*itr_proposal).owner, [&](auto& proposal) {
			proposal.status = proposal_status::ON_VOTE; //roll back to on vote status
		});
	}//action permission should be linked to a separate key


} //eosiowps