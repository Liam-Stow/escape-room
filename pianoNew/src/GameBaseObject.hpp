#pragma once
class GameBaseObject {
protected:
	// блочим копирование
	GameBaseObject(const GameBaseObject&) = delete;
	void operator=(const GameBaseObject&) = delete;
	GameBaseObject() {};
	virtual ~GameBaseObject() {};

	bool m_bWin = false;
	bool m_isActive = false;
	void(*m_onStateChangeCallback)(uint16_t elementNum, bool state) = nullptr;
	void(*m_onWinCallback)() = nullptr;
	

	void onStateChangedCallback(uint16_t elementNum, bool state) const
	{
		if (m_onStateChangeCallback != nullptr)
		{
			m_onStateChangeCallback(elementNum, state);
		}
	}
	
	void onWinCallback() const
	{
		if (m_onWinCallback != nullptr)
		{
			m_onWinCallback();
		}
	}

public:
	
	virtual bool process() = 0;
	virtual void onWin() = 0;
	virtual void reset() = 0;

	virtual bool isWin() const
	{
		return m_bWin;
	}

	virtual void setWin( bool win = true)
	{
		m_bWin = win;
	}
	
	virtual bool isActive() const
	{
		return m_isActive;
	}
	
	virtual void activate()
	{
		m_isActive = true;
	}

	virtual void deactivate()
	{
		m_isActive = false;
	}
	
	virtual void forceWin()
	{
		setWin();
		onWin();
		onWinCallback();
	}

	virtual void onChangedStateHandler(void(*ptrFunction)(uint16_t, bool))
	{
		m_onStateChangeCallback = ptrFunction;
	}
	
	virtual void onWinHandler(void(*ptrFunction)())
	{
		m_onWinCallback = ptrFunction;
	}

	
	


};
