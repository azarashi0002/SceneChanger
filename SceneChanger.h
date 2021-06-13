#pragma once

#include<Siv3D.hpp>
#include"Fwd.h"
#include"Scenes.h"
#include"Transitions.h"

namespace SceneChanger_ {
	class SceneChanger {
		Array<std::unique_ptr<Scenes::IScene>> scenes_;
		Optional<int64> before_index_, after_index_;
		std::unique_ptr<Transitions::ITransition> transition_ = TransitionFactory::Create<Transitions::CrossFade>(1s);
	public:
		SceneChanger() = default;
		SceneChanger(
			std::unique_ptr<Scenes::IScene>&& scene,
			std::unique_ptr<Transitions::ITransition>&& transition = nullptr) {
			change(std::move(scene), std::move(transition));
		}

		void setTransition(std::unique_ptr<Transitions::ITransition>&& transition) {
			if (not transition) { return; }

			transition_ = std::move(transition);
		}
		void change(std::unique_ptr<Scenes::IScene>&& next, std::unique_ptr<Transitions::ITransition>&& transition) {
			if (not next) { return; }

			if (after_index_) {
				scenes_.dropBack(scenes_.size() - 1 - *after_index_);
			}

			scenes_ << std::move(next);

			before_index_ = after_index_;
			if (after_index_) {
				++(*after_index_);
			}
			else {
				after_index_ = 0;
			}

			if (after()) {
				after()->initialize();
			}

			setTransition(std::move(transition));
		}
		void redo(std::unique_ptr<Transitions::ITransition>&& transition) {
			if (not after_index_) { return; }
			if (scenes_.size() <= *after_index_ + 1) { return; }

			before_index_ = after_index_;
			++(*after_index_);

			if (after()) {
				after()->initialize();
			}

			setTransition(std::move(transition));
		}
		void undo(std::unique_ptr<Transitions::ITransition>&& transition) {
			if (not after_index_) { return; }
			if (*after_index_ <= 0) { return; }

			before_index_ = after_index_;
			--(*after_index_);

			if (after()) {
				after()->initialize();
			}

			setTransition(std::move(transition));
		}

		bool update() {
			if (transition_) {
				transition_->update(before(), after());
			}

			if (after()) {
				if (after()->request_.change_) {
					change(
						std::move(after()->request_.change_->first),
						std::move(after()->request_.change_->second)
					);
				}
				if (after()->request_.undo_) {
					undo(std::move(*after()->request_.undo_));
				}
				if (after()->request_.redo_) {
					redo(std::move(*after()->request_.redo_));
				}
			}
			if (after()) {
				after()->request_.resetOptional();
			}
			if (before()) {
				before()->request_.resetOptional();
			}

			if (transition_) {
				if (auto&& i = transition_->nextTransition(); i.has_value()) {
					setTransition(std::move(*i));
				}
			}

			return after() ? not std::exchange(after()->request_.exit_, false) : true;
		}
		void draw() const {
			if (transition_) {
				transition_->draw(before(), after());
			}
		}

	private:
		std::unique_ptr<SceneChanger_::Scenes::IScene>& before() {
			static std::unique_ptr<SceneChanger_::Scenes::IScene> nul{ nullptr };
			return before_index_ ? scenes_[*before_index_] : nul;
		}
		const std::unique_ptr<SceneChanger_::Scenes::IScene>& before() const {
			static std::unique_ptr<SceneChanger_::Scenes::IScene> nul{ nullptr };
			return before_index_ ? scenes_[*before_index_] : nul;
		}
		std::unique_ptr<SceneChanger_::Scenes::IScene>& after() {
			static std::unique_ptr<SceneChanger_::Scenes::IScene> nul{ nullptr };
			return after_index_ ? scenes_[*after_index_] : nul;
		}
		const std::unique_ptr<SceneChanger_::Scenes::IScene>& after() const {
			static std::unique_ptr<SceneChanger_::Scenes::IScene> nul{ nullptr };
			return after_index_ ? scenes_[*after_index_] : nul;
		}
	};
} //namespace SceneChanger_
