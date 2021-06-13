#pragma once

#include<Siv3D.hpp>
#include"Fwd.h"

namespace SceneChanger_ {
	namespace Scenes {
		class IScene {
			friend class SceneChanger;

			struct {
				bool exit_ = false;
				Optional<std::pair<std::unique_ptr<IScene>, std::unique_ptr<Transitions::ITransition>>> change_;
				Optional<std::unique_ptr<Transitions::ITransition>> undo_;
				Optional<std::unique_ptr<Transitions::ITransition>> redo_;

				void resetOptional() {
					change_.reset();
					undo_.reset();
					redo_.reset();
				}
			} request_;
		public:
			virtual ~IScene() {}
			virtual void initialize() {}	//ÉVÅ[ÉìÇ™åƒÇŒÇÍÇΩÇ∆Ç´(undoÅEredoÇ≈Ç‡åƒÇŒÇÍÇÈ)

			virtual void update() = 0;
			virtual void draw() const = 0;

			virtual void updateFadeIn(double /*t*/) { update(); }
			virtual void updateFadeOut(double /*t*/) { update(); }
			virtual void drawFadeIn(double /*t*/) const { draw(); }
			virtual void drawFadeOut(double /*t*/) const { draw(); }

			void exit() {
				request_.exit_ = true;
			}
			void changeScene(std::unique_ptr<IScene>&& scene,
				std::unique_ptr<Transitions::ITransition>&& transition = nullptr) {
				request_.change_ = std::make_pair(std::move(scene), std::move(transition));
			}
			void undo(std::unique_ptr<Transitions::ITransition>&& transition) {
				request_.undo_ = std::move(transition);
			}
			void redo(std::unique_ptr<Transitions::ITransition>&& transition) {
				request_.redo_ = std::move(transition);
			}
		};
	}
} //namespace SceneChanger_
