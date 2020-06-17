



struct Impulse
{
	ChannelName channel;
	Number magnitude;
};

struct ImpulseTrigger
{
	Number release;
	AudioChannel channel;
	Seconds period;
};


struct ImpulseChannel
{
	ChannelName channel;
	Number magnitude;

	// threshold -> trigger
	Map<Number, ImpulseTrigger> triggers;
};

