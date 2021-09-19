#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Ellipse
{
	private:
	double dSma; // Eccentricity
	olc::vd2d f1, f2; // Ellipse foci
	double dC; // Distance between foci
	double dP; // Semilatius rectum
	double dEcc; // Eccentricity
	double domega; // Rotation angle of ellipse

	public:
	Ellipse(){};
	Ellipse(double dSmaIn, olc::vd2d f1In, olc::vd2d f2In)
	{
		dSma = dSmaIn;
		f1 = f1In;
		f2 = f2In;
		olc::vd2d vDiff = f2-f1;
		dC = 0.5*vDiff.mag();
		dP = (dSma*dSma-dC*dC)/dSma;
		dEcc = dC/dSma;
		domega = atan2(vDiff.y,vDiff.x);
	}

	olc::vd2d getPoint(double dTheta)
	{
		double dR = dP/(1.0-dEcc*cos(dTheta));
		olc::vd2d pointOut;
		pointOut.x = dR*cos(dTheta);
		pointOut.y = dR*sin(dTheta);

		// Rotate ellipse
		olc::vd2d pointOutRot;
		pointOutRot.x = pointOut.x*cos(domega) - pointOut.y*sin(domega) + f1.x;
		pointOutRot.y = pointOut.x*sin(domega) + pointOut.y*cos(domega) + f1.y;

		return pointOutRot;
		
	}
};

class SpaceCommandGame : public olc::PixelGameEngine
{
public:
	SpaceCommandGame()
	{
		sAppName = "Space Command";
	}

private:
	const unsigned nPoint = 1e4;
	olc::vd2d vCenter;
	float fScale;
	olc::vd2d vf1;
	olc::vd2d vf2;
	Ellipse elpA;
	double dSma;
	bool bMovingFocus = false;

public:
	bool OnUserCreate() override
	{
        vCenter = { (float)ScreenWidth() / 2, (float)ScreenHeight() / 2 };
		fScale = (float)ScreenWidth() / 4;

		// Initial ellipse definition
		dSma = 0.6;
		vf1 = {0.0, 0.0};
		vf2 = {0.5, 0.0};

		elpA = {dSma, vf1, vf2};
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		// USER INPUTS
		bool bRedoEllipse = false;
		if (GetMouseWheel() > 0)
		{
			dSma *= 1.05;
			bRedoEllipse = true;
		}
		if (GetMouseWheel() < 0)
		{
			dSma /= 1.05;
			bRedoEllipse = true;
		}

		if (GetMouse(0).bPressed)
		{
			olc::vi2d vf2ScreenPos = vf2*fScale+vCenter;
			olc::vi2d vDist;
			vDist.x = GetMouseX() - vf2ScreenPos.x;
			vDist.y = GetMouseY() - vf2ScreenPos.y;
			double dDist = vDist.mag();
			if (dDist < 0.05*fScale) bMovingFocus = true;
		}

		if (bMovingFocus)
		{
			olc::vd2d vf2New;
			vf2New.x = (GetMouseX()-vCenter.x)/fScale;
			vf2New.y = (GetMouseY()-vCenter.y)/fScale;
			vf2 = vf2New;
			bRedoEllipse = true;
		}

		if (GetMouse(0).bReleased) bMovingFocus = false;

		if (bRedoEllipse) elpA = {dSma, vf1, vf2};
		bRedoEllipse = false;

		// RENDERING
		Clear(olc::BLACK);

		// Draw foci
		FillCircle(vf1*fScale+vCenter, 0.05*fScale, olc::GREEN);
		FillCircle(vf2*fScale+vCenter, 0.05*fScale, olc::RED);

		// Draw ellipse
		for (unsigned idxPoint=0; idxPoint < nPoint; idxPoint++)
		{
			double dTheta = (double)(idxPoint)/nPoint*2*M_PI;
			Draw(elpA.getPoint(dTheta)*fScale+vCenter, olc::WHITE);
		}
		return true;
	}
};


int main()
{
	SpaceCommandGame sgame;
	if (sgame.Construct(1920/2, 1080/2, 1, 1))
		sgame.Start();

	return 0;
}
