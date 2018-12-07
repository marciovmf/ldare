struct Animation
{
	float elapsedTime;
	float totalTime;
	float totalElapsedTime;
	uint32 numFrames;
	uint32 currentFrameIndex;
	uint32 finished;
	Rectangle* frames;
	float timePerFrame;
	bool once;
};

//---------------------------------------------------------------------------
// Start an animation
//---------------------------------------------------------------------------
Animation* animation_start(Animation& animation)
{
		animation.elapsedTime = 0;
		animation.currentFrameIndex = 0;
		animation.totalElapsedTime = 0;
		animation.finished = false;
		return &animation;
}

//---------------------------------------------------------------------------
// Updates an animation
//---------------------------------------------------------------------------
inline Rectangle& animation_update(Animation& animation, float deltaTime)
{
		if(deltaTime < 0) 
		{
			deltaTime = +deltaTime;	
			LogError("animation speed is not supposed to be less than zero");
		}

		animation.elapsedTime += deltaTime;
		animation.totalElapsedTime += deltaTime;

		if ( animation.totalElapsedTime >= animation.totalTime && animation.once)
		{
			animation.finished = MIN(++animation.finished,1);
		}

		if (animation.elapsedTime > animation.timePerFrame)
		{
			animation.elapsedTime -= animation.timePerFrame;
			uint32 nextFrameIndex = (animation.currentFrameIndex + 1) % animation.numFrames;
			animation.currentFrameIndex = nextFrameIndex;
		}

	return animation.frames[animation.currentFrameIndex];
}


