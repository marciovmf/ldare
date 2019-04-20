
static struct Character
{
	Sprite sprite;
	Animation* animation;
	float speed;
	Vec3 direction;
	float elaspedChopTime;
	bool isChopping;
	bool isWalking;
} character;

static Animation _walkAnimationRight, _walkAnimationLeft,_choppAnimationRight, 
								 _choppAnimationLeft, _idleAnimationRight, _idleAnimationLeft; 

static Rectangle _srcIdleRight[] = 
{
	{ 0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
};

static Rectangle _srcIdleLeft[] = 
{
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
};

static Rectangle _srcWalkRight[] = 
{
	{ 0 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 3 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
};

static Rectangle _srcWalkLeft[] = 
{
	{ 1 * SPRITE_SIZE, 4 * SPRITE_SIZE, -SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 4 * SPRITE_SIZE, -SPRITE_SIZE, SPRITE_SIZE},
	{ 3 * SPRITE_SIZE, 4 * SPRITE_SIZE, -SPRITE_SIZE, SPRITE_SIZE},
	{ 4 * SPRITE_SIZE, 4 * SPRITE_SIZE, -SPRITE_SIZE, SPRITE_SIZE},
};

static Rectangle _srcChoppRight[] = 
{
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
	{ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE},
};

static Rectangle _srcChoppLeft[] = 
{
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 3 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 4 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
	{ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE,-SPRITE_SIZE, SPRITE_SIZE},
};


void character_setup(Character& character, float width, float height, Vec3& position, Vec4& color)
{
	// Set up walk animation right
	_walkAnimationRight.elapsedTime=0;
	_walkAnimationRight.numFrames = 4;
	_walkAnimationRight.totalTime = 0.4f;
	_walkAnimationRight.frames = _srcWalkRight;
	_walkAnimationRight.timePerFrame = _walkAnimationRight.totalTime / _walkAnimationRight.numFrames;
	// walk animation left
	_walkAnimationLeft = _walkAnimationRight;
	_walkAnimationLeft.frames = _srcWalkLeft;

	// idle animation left
	_idleAnimationLeft.numFrames = 12;
	_idleAnimationLeft.totalTime = 2.0f;
	_idleAnimationLeft.frames = _srcIdleLeft;
	_idleAnimationLeft.timePerFrame = _idleAnimationLeft.totalTime / _idleAnimationLeft.numFrames;

	// idle animation right
	_idleAnimationRight.numFrames = 12;
	_idleAnimationRight.totalTime = 2.0f;
	_idleAnimationRight.frames = _srcIdleRight;
	_idleAnimationRight.timePerFrame = _idleAnimationRight.totalTime / _idleAnimationRight.numFrames;

	// Chopp animation left
	_choppAnimationLeft = _walkAnimationRight;
	_choppAnimationLeft.numFrames = 3;
	_choppAnimationLeft.totalTime = 0.3f;
	_choppAnimationLeft.timePerFrame = _choppAnimationLeft.totalTime / _choppAnimationLeft.numFrames;
	_choppAnimationLeft.frames = _srcChoppLeft;
	_choppAnimationLeft.once = true;

	// Chop animation Right
	_choppAnimationRight = _walkAnimationRight;
	_choppAnimationRight.numFrames = 3;
	_choppAnimationRight.totalTime = 0.3f;
	_choppAnimationRight.timePerFrame = _choppAnimationRight.totalTime / _choppAnimationRight.numFrames;
	_choppAnimationRight.frames = _srcChoppRight;
	_choppAnimationRight.once = true;

	Sprite sprite;
	sprite.color = color;
	sprite.width =  width;
	sprite.height = height;
	sprite.position = position;

	character.sprite = sprite;
	character.animation = &_idleAnimationRight;
	character.direction = Vec3{1,0,0};
}

static float colorSum = 0;
static void updateIdleState(Character& character, float deltaTime)
{
	if (character.animation != &_idleAnimationLeft && character.animation != &_idleAnimationRight)
	{
		if (character.direction.x > 0|| character.direction.y > 0)
			character.animation = &_idleAnimationRight;
		else
			character.animation = &_idleAnimationLeft;
		animation_start(*character.animation);
	}

	colorSum += deltaTime * 5;
	//character.sprite.color.x = sin(colorSum);

	character.isWalking = character.isChopping = false;
	character.sprite.srcRect = animation_update(*character.animation, deltaTime);
}

static void updateWalkState(Character& character, float deltaTime)
{
	Animation* nextAnimation = character.animation;

	if (character.direction.x > 0)
	{
		nextAnimation = &_walkAnimationRight;
	}
	else if (character.direction.x < 0)
	{
		nextAnimation = &_walkAnimationLeft;
	}
	else
	{
		// Diagonals
		if ( character.direction.y > 0)
			nextAnimation = &_walkAnimationRight;
		else
			nextAnimation = &_walkAnimationLeft;
	}

	character.isWalking = true;
	character.isChopping = false;
	// Set a new animation or keep playing the current one
	if (nextAnimation != character.animation)
	{
		character.animation = nextAnimation;
		animation_start(*character.animation);
	}
	character.sprite.srcRect = animation_update(*character.animation, deltaTime);
}

void updateChopState(Character& character, float deltaTime)
{
	if ( character.animation != &_choppAnimationLeft && character.animation != &_choppAnimationRight )
	{
		character.animation = (character.direction.x > 0 || character.direction.y > 0) ?
			&_choppAnimationRight :&_choppAnimationLeft;

		animation_start(*character.animation);
	}

	character.isChopping = character.animation->finished == 0;
	character.isWalking = false;
	character.sprite.srcRect = animation_update(*character.animation, deltaTime);
}

void character_update(Character& character, const Input& input, GameApi& gameApi, float deltaTime)
{	
	float axisX = input.getAxis(GAMEPAD_AXIS_LX);
	float axisY = input.getAxis(GAMEPAD_AXIS_LY);
	character.speed = SPRITE_SIZE * deltaTime;

	/// -----------------------------------------------------
	// Getting input
	// -----------------------------------------------------
	Vec3 direction = {};
	// right
	bool right = ( input.getKey(KBD_D) 
			|| input.getButton(GAMEPAD_DPAD_RIGHT) 
			|| (axisX > 0.0f)); 

	// left
	bool left = ( input.getKey(KBD_A) 
			|| input.getButton(GAMEPAD_DPAD_LEFT) 
			||(axisX < 0.0f));

	// up
	bool up = (input.getKey(KBD_W) 
			|| input.getButton(GAMEPAD_DPAD_UP) 
			|| (axisY > 0.0f));

	// down
	bool down = ( input.getKey(KBD_S) 
			|| input.getButton(GAMEPAD_DPAD_DOWN) 
			|| (axisY < 0.0f));

	bool actionBtn = (input.getKey(KBD_J) 
			|| input.getButton(GAMEPAD_A));

	// -----------------------------------------------------
	// Set character facing direction
	// -----------------------------------------------------
	if (up)
	{
		direction.y = 1;
	}
	else if (down)
	{
		direction.y = -1;
	}

	if (right)
	{
		direction.x = 1;
	}
	else if (left)
	{
		direction.x = -1;
	}

	bool walking = (up || down || left || right);
	bool chopping = character.isChopping || (actionBtn && !walking);

	// -----------------------------------------------------
	// Update animations
	// -----------------------------------------------------
	if (chopping)
	{
		updateChopState(character, deltaTime);
	}
	// Walking
	else if (walking)
	{
		character.direction = direction;
		updateWalkState(character, deltaTime);
	}
	else // Idle
	{
		updateIdleState(character, deltaTime);
	}
}
