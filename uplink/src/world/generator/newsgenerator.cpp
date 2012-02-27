
#include <string.h>
#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "game/game.h"

#include "world/world.h"
#include "world/agent.h"
#include "world/company/company.h"
#include "world/company/news.h"
#include "world/company/companyuplink.h"
#include "world/computer/computer.h"

#include "world/generator/numbergenerator.h"
#include "world/generator/newsgenerator.h"

#include "mmgr.h"



void NewsGenerator::Initialise ()
{

}

void NewsGenerator::ComputerHacked ( Computer *comp, AccessLog *al )
{

	UplinkAssert (comp);
	UplinkAssert (al);

		/* ==========================================================
			Structure:

			Headline
			Part1 : Basic intro (eg ss databased hacked again)
			Part2 : Recent number of hacks
			[part3 : Date of attack]  ( automatic )
			part4 : Future or existing effects of hack
			part5 : Promised action
			part6 : Misc

		   ========================================================== */

	std::ostrstream headline;
	std::ostrstream part1, part2, part3, part4, part5, part6;

	// ================================================================================================================
	// Social security database
	// ================================================================================================================

	if ( strcmp ( comp->name, "International Social Security Database" ) == 0 ) {

		switch ( comp->NumRecentHacks () ) {
			case 0:
			case 1:				headline << "Government social security records compromised";
								part1    << "The Government run social security database has been hacked by an unknown cyber criminal.";
								part2    << "Government officials insist this is the first and only incident of its kind.";
								part4	 << "In theory, unlawful changes to this database could result in dead people being reported as alive, "
											"or alive people being reported as dead.  Other personal information could be stolen.";
								part5	 << "A government federal agent today stated that this would not be allowed to happen again.";
								break;

			case 2:				headline << "Social security hacked twice in less than a month";
								part1	 << "The Government run social security database has once again been hacked by an unknown cyber criminal.  ";
								part2	 << "This is the second time in less than a month that this system has been attacked.  "
											"It is not known if these two incidents are connected.";
								part4	 << "A hacker gaining access to this system could do untold damage by modifying crucial personal data.";
								part5	 << "The government today launched an investigation into these recent security problems.";
								break;

			case 3:				headline << "Government held Files compromised for the third time";
								part1	 << "For the third time in less than a month, the government owned Social Security Database has been hacked.";
								part4	 << "The system holds key personal data on millions of people and has been the cause of some embarrasment recently "
											"for the government.";
								part5	 << "The potential for damage on this system is huge, prompting calls by opposition parties for a full "
											"review of all security protocols.";
								break;

			default:

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		headline << "Government Social Security files accessed again";
									part4    << "The system holds personal data on the general population, but could be maliciously used to create "
												"new identities or tamper with existing ones.";
									break;
					case 2 :		headline << "Social Security computer once again hacked";
									part4	 << "It is feared that the many recent hacks of this system may lead to a lack of trust in its data, which "
												"is used all over industry for many purposes.";
									break;
					case 3 :		headline << "Government maintained Social Security documents come under attack again";
									part4	 << "In theory, a skilled hacker could manufacture a new identity, or tamper with an existing record "
												"to cause damage to a person's records.  ";
									break;
				}

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		part2	 << "This system has been hacked multiple times in the last month, prompting heavy critisism from some opposition parties.";	break;
					case 2 :		part2	 << "This is not the first time this has happened; this system has been tampered with more than once recently.";				break;
					case 3 :		part2	 << "The Government is once again on the defensive as this system seems to be almost wide open to abuse.";						break;
				}

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		part1	 << "Government social security documents have been accessed once again.";
									part5	 << "Government spokesmen today insisted they had the situation under control, despite recent history.";
									break;
					case 2 :		part1    << "Government maintained Social Security documents have come under attack yet again.";
									part5	 << "The government is said to be considering many options on how to stop these attacks.";
									break;
					case 3 :		part1    << "The Goverment owned Social Security computer has once again been hacked by an unknown Cyber criminal.";
									part5	 << "Federal agents have been called in to help catch the criminals responsible for this action.";
									break;
				}

				break;

		}

	}

	// ================================================================================================================
	// Academic database
	// ================================================================================================================

	else if ( strcmp ( comp->name, "International Academic Database" ) == 0 ) {

		switch ( comp->NumRecentHacks () ) {
			case 0:
			case 1:				headline << "Government run academic database hacked";
								part1 << "The International Academic Database has been compromised by  malicious hacker.";
								part2 << "Until today this system was thought to be secure against this type of attack.";
								part4 << "The motives of the hacker are not clear but it is thought he was attempting to forge "
										 "qualifications for an unknown recipient.";
								part5 << "Government agents made it clear they would not tolerate this action and have already taken steps "
										 "to secure the database against future action.";
								break;

			case 2:				headline << "Academic records hacked again";
								part1 << "The International Academic Database has been hacked for the second time in less than a month.";
								part4 << "It is not known if the two attacks are connected.";
								part5 << "The government today began an investigation into these attacks.";
								break;

			case 3:				headline << "Government academic database hacked again";
								part1 << "The Government is on the defensive again today as its International Academic Database is hacked again.";
								part2 << "This attack comes less than a week after a recent hack and is the third this month.";
								part4 << "It is believed that an individual or small group is attempting to create new academic details and qualifications, "
										 "perhaps offering this as a service.";
								part5 << "Federal agents have been brought in to investigate.";
								break;

			default:

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		headline << "Government embarresed over repeated security failures";
									part5	 << "The Government has again promised to put a stop to these hacks quickly.";
									break;
					case 2 :		headline << "International Academic Database hacked again";
									part5	 << "The Government seems to be at a loss as to what to do about these recent attacks.";
									break;
					case 3 :		headline << "Federal Agents investigate Academic Database hacks";
									part5	 << "Federal Agents from the computer crimes division where today tasked with catching the criminals involved "
												"and determining what damage has been done.";
									break;
				}

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		part2	 << "Security on this computer has been bypassed repeadly in the last month.";			break;
					case 2 :		part2	 << "This attack is the latest in a string of hacks performed in the last month.";		break;
					case 3 :		part2	 << "Federal agents seem unable to stop a torrent of recent hacks on this system.";		break;
				}

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		part1	 << "The Government Academic Database has been hacked again by an unknown intruder.";
									part4	 << "The system is used to store the Academic history of millions of people.";
									break;
					case 2 :		part1	 << "Hackers have again penetrated the International Acadmic Database.";
									part4	 << "In theory a skilled hacker could create new qualifications for any person he wished.";
									break;
					case 3 :		part1	 << "The International Academic Database has been attacked by hackers again.";
									part4	 << "The exact motives of the hackers are not clear, but it is believed they may be running "
												"some kind of buisiness offering new qualifications for people for a price.";
				}

				break;

		}

	}

	// ================================================================================================================
	// Global Criminal Database
	// ================================================================================================================

    else if ( strcmp ( comp->name, "Global Criminal Database" ) == 0 ) {

        switch ( comp->NumRecentHacks () ) {

            case 0:
            case 1:             headline << "Global Criminal Records attacked by hackers";
                                part1 << "The Government managed Global Criminal Database has been penetrated by an unknown computer hacker.";
                                part2 << "Security on the system is so tight that hacks of this kind were thought to be impossible.";
                                part4 << "It appears from a preliminary investigation that an unknown criminal record was modified, prompting "
                                         "calls from human rights groups for higher security on personal records of this kind.";
                                part5 << "The Government has said they have begun an investigation and have taken steps to stop this from happening again.";
                                break;

            case 2:             headline << "Criminal Records opened to the public domain";
                                part1 << "Despite the installation of a brand new high security system, the Global Criminal Database has again been attacked "
                                         "and penetrated by a hacker or group of hackers.";
                                part2 << "Government agents are said to be amazed that their new system has failed so soon.";
                                part4 << "This is the second time criminal records have been modified by hackers masquerading as officers.";
                                part5 << "The newly installed system will no doubt be tested in light of this new breach.";
                                break;

            case 3:             headline << "Hackers take advantage of Global Criminal Database";
                                part1 << "The Government owned Global Criminal Database system has recently suffered a couple of high security hacks, "
                                         "and it would seem more and more hackers are taking advantage of the system.";
                                part2 << "For the third time in less than a month a hacker gained adminstrative access to the system and modified "
                                         "criminal records within the system.";
                                part4 << "The Government now believes a group of individuals are offering modifications to personal records such as criminal records "
                                         "for a price, perhaps advertising through the underground anarchist group Uplink Corporation.";
                                part5 << "The Government has now clamped down on accesses to the system, claiming it is safer than ever.";
                                break;

            default:

                switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
                    case 1 :        headline << "Global Criminal Database smashed open again";
                                    part5    << "The Government appears to be unable to put a stop to these attacks.";
                                    break;
                    case 2 :        headline << "Government criminal records system hacked yet again";
                                    part5    << "The Government appear to be at a loss as to what to do.";
                                    break;
                    case 3 :        headline << "Criminal Records modified by hackers again";
                                    part5    << "The Government is now considering shutting down the Global Criminal Database until this can be resolved.";
                                    break;
                }

                switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
                    case 1 :        part2    << "Security at this server system has been compromised numerous times in the past month.";                               break;
                    case 2 :        part2    << "This news service has unfortunately lost count of the number of hacks at this site in recent times.";                 break;
                    case 3 :        part2    << "The system appears to have more security holes than popular 'Micro software' written in the late 20th century.";      break;
                }

                switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
                    case 1 :        part1    << "The Government run Global Criminal Database has again been hacked wide open.";
                                    part4    << "It is believed an underground organisation is selling the service of modifying records.";
                                    break;
                    case 2 :        part1    << "The Global Criminal Database has been successfully attacked once again.";
                                    part4    << "Hackers could clear existing records or could have innocent people thrown in jail.";
                                    break;
                    case 3 :        part1    << "Once again the Global Criminal Database has been hacked.";
                                    part4    << "A number of cases have now arisen of people in prison who claim they were framed in this way.";
                                    break;
                }

                break;

        }

    }

	// ================================================================================================================
	// Bank Server
	// ================================================================================================================

	else if ( comp->TYPE == COMPUTER_TYPE_PUBLICBANKSERVER ) {

		switch ( comp->NumRecentHacks () ) {
			case 0 :
			case 1 :			headline << comp->name << " hacked";
								part1    << "The financial computer system " << comp->name << " has been penetrated from an unknown source.";
								part2	 << "This is the first incident of its kind for the owner, " << comp->companyname << ".";
								part4	 << "Hackers gaining access to this system could perform any number of fund transfers.";
								part5	 << comp->companyname << " are investigating the incident.";
								break;

			case 2 :			headline << comp->name << " comes under attack again";
								part1    << "The " << comp->name << " computer system has been hacked for the second time.";
								part4    << "It is believed that some unauthorised money transfers were performed by the hackers.";
								part5	 << "This incident has lead to a fall in investor trust, and  the Bank is currently searching for "
											"clues as to the identity of the hackers responsible.";
								break;

			case 3 :			headline << comp->name << " hacked for the third time";
								part1	 << comp->companyname << " are at a loss today as their online trading system, "
										 << comp->name << " was hacked for the third time in less than a month.";
								part4	 << "Funds are being illegally transfered but " << comp->companyname << " are unable to "
											"provide any further details at this time.";
								part5	 << "Earlier today they issued a statement, saying that Federal Agents had been brought in "
											"to try to catch these hackers before they cause serious damage.";
								break;

			default :

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		headline << comp->name << " is hacked again";
									part2	 << "This system has been hacked at least 4 times in the last month.";
									break;
					case 2 :		headline << comp->name << " security fails once again";
									part2	 << "This is the latest in a long list of security failures for " << comp->companyname << ".";
									break;
					case 3 :		headline << comp->name << " is penetrated by hackers again";
									part2	 << "Investor confidence is low as this system has been hacked many times recently.";
									break;
				}

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		part1	 << "The " << comp->name << " has been attacked and penetrated again by unknown cyber-criminals.";		break;
					case 2 :		part1	 << "Earlier today the " << comp->name << " system was attacked again by Computer hackers.";			break;
					case 3 :		part1	 << comp->companyname << " suffered another embarrasing security problem today as their "
											 << comp->name << " system was hacked from an unknown source.";
				}

				switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
					case 1 :		part4	 << "It is believed the hackers were attempting to transfer funds without authorisation.";
									part5	 << comp->companyname << " have hired freelance computer specialists in an attempt to catch the guilty parties.";
									break;
					case 2 :		part4	 << "Records indicate the hackers performed some kind of financial transfer, then attempted to remove "
											    "any traces of their actions.";
									part5	 << comp->companyname << " say they are are confident they will catch the hacker.";
									break;
					case 3 :		part4	 << "Hackers gaining access to this system could perform any number of fund transfers between accounts.";
									part5	 << comp->companyname << " are said to be considering further action in light of the number of recent hacks.";
									break;
				}

				break;

		}

	}

	else {

		// Not worth writing a story about it

		return;

	}

	//
	// Concatenate each part together and post it
	//

	part3 << "This occured on " << al->date.GetLongString ();

	headline << '\x0';
	part1 << '\x0';
	part2 << '\x0';
	part3 << '\x0';
	part4 << '\x0';
	part5 << '\x0';
	part6 << '\x0';

	std::ostrstream details;
	details << part1.str () << "\n\n" << part2.str () << "\n\n" << part3.str () << "\n\n"
			<< part4.str () << "\n\n" << part5.str () << "\n\n" << part6.str () << '\x0';

	News *news = new News ();
	news->SetHeadline ( headline.str () );
	news->SetDetails ( details.str () );
	news->SetData ( NEWS_TYPE_HACKED, comp->ip );

	headline.rdbuf()->freeze( 0 );
	part1.rdbuf()->freeze( 0 );
	part2.rdbuf()->freeze( 0 );
	part3.rdbuf()->freeze( 0 );
	part4.rdbuf()->freeze( 0 );
	part5.rdbuf()->freeze( 0 );
	part6.rdbuf()->freeze( 0 );
	details.rdbuf()->freeze( 0 );
	//delete [] headline.str ();
	//delete [] part1.str ();
	//delete [] part2.str ();
	//delete [] part3.str ();
	//delete [] part4.str ();
	//delete [] part5.str ();
	//delete [] part6.str ();
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void NewsGenerator::ComputerDestroyed ( Computer *comp, bool filesdeleted )
{

	UplinkAssert ( comp );

		/* ==========================================================
			Structure

			headline
			Part1		:		Who was broken into, everything destroyed
			Part2		:		Details of destruction
			part2b		:		(If applicable) Files recovered
			Part3		:		Consequences
			Part4		:		Actions

		   ========================================================== */

	std::ostrstream headline;
	std::ostrstream part1, part2, part3, part4;

	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :			headline << "Hackers take down major computer system";						break;
		case 2 :			headline << "Large corporation stung by malicious hacker attack";			break;
		case 3 :			headline << "Aggressive computer attacker leaves system crippled";			break;
	}

	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :			part1 << comp->companyname << " have taken a major blow to their operations today as a malicious "
							"computer hacker penetrated one of their key computer systems and caused terminal damage.";
							break;

		case 2 :			part1 << "A lone hacker has penetrated one of the primary computer systems of "
							<< comp->companyname << " and deliberately shut down the system.";
							break;

		case 3 :			part1 << "Federal agents are now searching for the identity of a computer criminal "
							"who broke into the " << comp->name << " and deliberately destroyed the machine.";
							break;

	}

	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :			part2 << "It has been difficult to piece together exactly what happened, but it appears "
							"that a system wide failure of the computer system was caused by the hackers actions.  "
							"Experts say the entire operating system will have to be replaced.";
							break;

		case 2 :			part2 << "Industry experts have re-created the final moments of the system crash from "
							"access logs on another machine.  A few moments before the computer shut down, a lone "
							"hacker forced his way into the system after bypassing the security, entered into "
							"administrative mode and deleted several key system files, causing the machine to malfunction.";
							break;

		case 3 :			part2 << "A single hacker is responsible for this high-tech crime.  Moments before the failure "
							"he hacked into the system and deliberatly brought about its destruction.  The exact method has "
							"not yet been determined.";
							break;

	}

	if ( filesdeleted ) {

		part2 << "\nThe company stated in their press release that all files had been totally destroyed.";

	}
	else {

		part2 << "\nExtensive damage was done but the company has been able to salvage most of the data.";

	}

	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :			part3 << "The city witnessed a sharp drop in the share price today as news of this disaster "
							"reached investors.  It seems that nobody is safe after such a high profile system is destroyed.  "
							<< comp->companyname << " said that millions of credits of damage had been done already.";
							break;

		case 2 :			part3 << "This system failure will cost the company dearly.  It seems the data on the system is "
							"lost for good, and the costs of repairing this damage could spiral into millions of dollars.  "
							"Industry analysts say the company is looking at some rough times ahead.";
							break;

		case 3 :			part3 << "Today the company chairman tried to play down the significance of this event, no doubt "
							"trying to avoid a public relations disaster as well as a system failure.  There can be little "
							"doubt that this set back will cost the company dearly.";
							break;

	}

	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :			part4 << "Company executives today released a press statement, indicating they were investigating "
							"the incident and would be seeking outside assistance from industry professionals.";
							break;

		case 2 :			part4 << "Federal agents have been brought in on the case and will no doubt be searching for "
							"the identity of this dangerous computer hacker.";
							break;

		case 3 :			part4 << comp->companyname << " have tightened security on their remaining computer systems, "
							"and federal agents from the government cyber-crimes division were seen entering the corporate "
							"headquarters today.";
							break;

	}


	//
	// Concatenate each part together and post it
	//

	headline << '\x0';
	part1 << '\x0';
	part2 << '\x0';
	part3 << '\x0';
	part4 << '\x0';

	std::ostrstream details;
	details << part1.str () << "\n\n" << part2.str () << "\n\n" << part3.str () << "\n\n" << part4.str () << '\x0';

	News *news = new News ();
	news->SetHeadline ( headline.str () );
	news->SetDetails ( details.str () );

	if ( filesdeleted ) news->SetData ( NEWS_TYPE_COMPUTERDESTROYED, comp->ip );
	else				news->SetData ( NEWS_TYPE_COMPUTERSHUTDOWN, comp->ip );

	headline.rdbuf()->freeze( 0 );
	part1.rdbuf()->freeze( 0 );
	part2.rdbuf()->freeze( 0 );
	part3.rdbuf()->freeze( 0 );
	part4.rdbuf()->freeze( 0 );
	details.rdbuf()->freeze( 0 );
	//delete [] headline.str ();
	//delete [] part1.str ();
	//delete [] part2.str ();
	//delete [] part3.str ();
	//delete [] part4.str ();
	//delete [] details.str ();


	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void NewsGenerator::AllFilesStolen ( Computer *comp, char *filetype, int totalfilesize )
{

	UplinkAssert (comp);

	// Filetype is SCIENTIFIC, CORPORATE, CUSTOMER or SOFTWARE

	/* =======================================================
		Structure:

		Headline
		Part1		:		Who was broken into, all files stolen
		Part2		:		Possible use of computer / type of data / Total size
		Part3		:		Consequences / Actions

		====================================================== */

	std::ostrstream headline;
	std::ostrstream part1, part2, part3;

    totalfilesize = 10 * int(totalfilesize/10);

	switch ( comp->NumRecentHacks () ) {

		case 0:
		case 1:				part1 << comp->companyname << " discovered this morning that a massive quantity of computer data "
									"had been stolen from one of their Central Mainframes.";
							break;

		case 2:				part1 << "A computer hacker entered the Central File Server of " << comp->companyname << " today and "
									"copied a huge portion of computer data to an unknown location.  This incident comes less than "
									"a month after a similar incident where their central mainframe was compromised.";
							break;

		case 3:				part1 << comp->companyname << " are once again reviewing their security procedures after revealing that "
									"their central mainframe has been compromised for the third time in a month, with massive amounts "
									"of confidential data being transfered away.";
							break;

		default:			part1 << comp->companyname << " have suffered yet another embarrassing security breach, today revealing "
									"that a massive quantity of private data was stolen from their central mainframe.  This system has "
									"been targetted repeatedly in recent times.";

	};

	if ( strcmp ( filetype, "SCIENTIFIC" ) == 0 ) {

		headline << "Gigaquads of scientific data copied by hacker";

		part2 << "The stolen files contained data from a massive scientific research program, which " << comp->companyname
			  << " had been working on for the past 3 years.\n"
              << "Around " << totalfilesize << " Gigaquads of data was copied before the intruder disconnected.";
		part3 << comp->companyname << " will undoubtedly try to discover the corporation behind this theft.";

	}
	else if ( strcmp ( filetype, "CORPORATE" ) == 0 ) {

		headline << "No data is secure - GigaQuads stolen in 'impossible' hack";

		part2 << "It is believed around " << totalfilesize << " Gigaquads of confidential "
				 "corporate data was stolen by this hacker, and it was probably copied to a temporary fileserver somewhere "
				 "on the web.";

		part3 << comp->companyname << " have said that they are considering all options to catch this hacker, and "
				 "the corporation that paid him.";

	}
	else if ( strcmp ( filetype, "CUSTOMER" ) == 0 ) {

		headline << "Personal customer data copied by hacker";

		part2 << "The computer system stored hundreds of GigaQuads of personal data on all of the corporation's customers.  "
				 "This data is now in the public domain.  Over " << totalfilesize << " Gigaquads of data was stolen.";

		part3 << comp->companyname << " tried to reassure the public today, and issued a statement saying they had brought "
				 "in Federal Agents to try and find the corporation responsible.";

	}
	else if ( strcmp ( filetype, "SOFTWARE" ) == 0 ) {

		headline << "Two years of software development - stolen";

		part2 << comp->companyname << " had been working on several advanced software products and this computer system "
				"was their primary file server.  Most of their proprietary source code has been copied - over "
                << totalfilesize << " Gigaquads of data, immediately "
				"compromising several of their planned security systems.";

		part3 << "Federal Agents have been called in to help track down the Corporation and the hacker responsible.";

	}


	//
	// Concatenate each part together and post it
	//

	headline << '\x0';
	part1 << '\x0';
	part2 << '\x0';
	part3 << '\x0';

	std::ostrstream details;
	details << part1.str () << "\n\n" << part2.str () << "\n\n" << part3.str () << '\x0';

	News *news = new News ();
	news->SetHeadline ( headline.str () );
	news->SetDetails ( details.str () );
	news->SetData ( NEWS_TYPE_STOLEN, comp->ip );

	headline.rdbuf()->freeze( 0 );
	part1.rdbuf()->freeze( 0 );
	part2.rdbuf()->freeze( 0 );
	part3.rdbuf()->freeze( 0 );
	details.rdbuf()->freeze( 0 );
	//delete [] headline.str ();
	//delete [] part1.str ();
	//delete [] part2.str ();
	//delete [] part3.str ();
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void NewsGenerator::AllFilesDeleted ( Computer *comp, char *filetype )
{

	// Filetype is SCIENTIFIC, CORPORATE, CUSTOMER or SOFTWARE

	UplinkAssert (comp);

		/* ===========================================================
			Structure:

			Headline
			Part1		:		Who was broken into, all files where deleted
			Part2		:		Type of data / use of computer
			Part3		:		Consequences / actions

		   =========================================================== */

	std::ostrstream headline;
	std::ostrstream part1, part2, part3;


	switch ( comp->NumRecentHacks () ) {

		case 0:
		case 1:				part1 << comp->companyname << " found themselves in the public eye today as one of their "
								  "primary computer systems suffered a complete failure, resulting in a large amount "
								  "of lost data.";
							break;

		case 2:				part1 << comp->companyname << " again found themselves in trouble as gigaquds of data where "
								  "wiped from their system.  In a statement today the company claimed it had evidence "
								  "that the files where deliberately deleted by a malicious hacker.";
							break;

		default:			part1 << comp->companyname << " are today boosting their security systems as their file servers "
								  "suffered yet another complete failure.  The company has come under attack on several "
								  "occasions in the last month by hackers who have yet to be identified.";
							break;

	};

	if ( strcmp ( filetype, "SCIENTIFIC" ) == 0 ) {

		headline << "Computer failure blaimed for research loss";

		part2 << "The data deleted was the combined results of a large scale scientific test, being conducted "
				 "by " << comp->companyname << " in order to develop new technology.";

		part3 << "In a statement today the company said that serious damage had been done and that it "
				 "could cost the company millions of dollars to perform the research again.";

	}
	else if ( strcmp ( filetype, "CORPORATE" ) == 0 ) {

		headline << "Major corporation suffers complete data loss";

		part2 << "The company found that masses of corporate data has been destroyed.";

		part3 << "It is not known how much this will cost the company but there is little doubt that serious damage "
				 "has been done.  This level of data loss could not occur without serious repurcusions.";

	}
	else if ( strcmp ( filetype, "CUSTOMER" ) == 0 ) {

		headline << comp->companyname << " loses gigaquads of customer data due to 'malicious hackers'";

		part2 << "The computer system that was damaged was used to hold customer records and sales data.  It is "
			     "believed that all backups of the data were also destroyed, leading the company to believe that "
				 "this was a deliberate act.";

		part3 << comp->companyname << " today issued a statement saying that they were investigating the incident.";

	}
	else if ( strcmp ( filetype, "SOFTWARE" ) == 0 ) {

		headline << "Proprietary software source code deleted by hacker";

		part2 << "Upon investigating the damaged computer, the company found that much of their software that was "
			     "still in development had been deleted, along with all source code and back ups.  It was claimed "
				 "that the revolutionary software would dominate the market once released.";

		part3 << comp->companyname << " have undoubtably suffered a large loss here, which could cost the company "
				 "millions.";

	}


	//
	// Concatenate each part together and post it
	//

	headline << '\x0';
	part1 << '\x0';
	part2 << '\x0';
	part3 << '\x0';

	std::ostrstream details;
	details << part1.str () << "\n\n" << part2.str () << "\n\n" << part3.str () << '\x0';

	News *news = new News ();
	news->SetHeadline ( headline.str () );
	news->SetDetails ( details.str () );
	news->SetData ( NEWS_TYPE_DELETED, comp->ip );

	headline.rdbuf()->freeze( 0 );
	part1.rdbuf()->freeze( 0 );
	part2.rdbuf()->freeze( 0 );
	part3.rdbuf()->freeze( 0 );
	details.rdbuf()->freeze( 0 );
	//delete [] headline.str ();
	//delete [] part1.str ();
	//delete [] part2.str ();
	//delete [] part3.str ();
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void NewsGenerator::Arrested ( Person *person, Computer *comp, char *reason )
{

	UplinkAssert ( person );
	UplinkAssert ( reason );
	// comp can be NULL


		/* ==========================================================
			Structure:

			Headline
			Part1		:		Intro and basic overview
			part2		:		Background of person
			part3		:		World reaction / consequences

		   ========================================================== */

	std::ostrstream headline;
	std::ostrstream part1, part2, part3;

	part1 << "Earlier today a man was arrested by Federal Agents for " << reason;

	// ================================================================================================================
	// Civilian caught
	// ================================================================================================================

	if ( person->rating.uplinkrating == 0 ) {

		switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
			case 1 :			headline << "Man arrested for Computer Crime";							break;
			case 2 :			headline << "Police arrest suspect in Computer fraud crime";			break;
			case 3 :			headline << "Federal agents arrest suspect in dawn raid";				break;
		}

		part2 << "The man arrested, who has been named as " << person->name << ", has protested his innocence "
				 "from the start, claiming he has no knowledge of Computers at all and could not have "
				 "committed this crime.";

		switch ( NumberGenerator::RandomNumber ( 2 ) + 1 ) {
			case 1 :			part3 << "The man will face court in a few months time for these crimes and could serve "
										 " several years in jail for this crime.";
								break;
			case 2 :			part3 << "This case will go to court and he may face some time in jail.  "
										 "Friends and family today began a campaign to prove his innocence, claiming "
										 "that he does not have the knowledge or equipment necessary to complete "
										 "the crimes he is accused of.";
								break;
		}

	}

	// ================================================================================================================
	// Amateur hacker caught
	// ================================================================================================================

	else if ( person->rating.uplinkrating < 3 ) {

		switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
			case 1 :			headline << "Amateur hacker caught and arrested";						break;
			case 2 :			headline << "Federal Agents catch hacker red handed";					break;
			case 3 :			headline << "Part-time hacker brought to justice";						break;
		}

		part2 << "The man has been named by Federal Agents as " << person->name << ", and is on record with a "
				 "history of small time computer crime.  Police who arrested him found Computer equipment and "
				 "hardware for hacking in his house.  It is believed this man was a part-time employee of the "
				 "underground hacker group Uplink-Corporation, where he went by the hacker alias of "
				 << ((Agent *) person)->handle << ".";

		part3 << "He may not get a chance to hack again however, as he could face up to ten years in prison.";

	}

	// ================================================================================================================
	// Experienced hacker caught
	// ================================================================================================================

	else if ( person->rating.uplinkrating < 11 ) {

		switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
			case 1 :			headline << "Experienced computer hacker finally caught";				break;
			case 2 :			headline << "Federal Agents catch up with underground Hacker";			break;
			case 3 :			headline << "High-ranked Uplink Agent arrested";						break;
		}

		part2 << "The man's real name is " << person->name << " and he is a well known player in the underground "
				 "hacker scene.  It is believed he is a full-time freelance hacker, working for several organised "
				 "groups including the well known Uplink-Corporation, where he went by the hacker alias of "
				 << ((Agent *) person)->handle << ".";

		part3 << "Police suspect this man of multiple computer crimes and as such it is believed he will serve "
			     "between ten and fifteen years inprisonment, and most likely a life ban on the use of computers.";


	}

	// ================================================================================================================
	// Hacker demi-god caught
	// ================================================================================================================

	else {

		switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {
			case 1 :			headline << "Hacker ring-leader arrested in dawn raid";					break;
			case 2 :			headline << "Police arrest hacker 'Demi-God'";							break;
			case 3 :			headline << "Federal Agents pleased with arrest of hacker leader";		break;
		}

		part2 << "The man in question is " << person->name << " and he is a well established member of the secretive "
				 "underground hacker movement Uplink-Corporation, where he worked behind the hacker alias of "
				 << ((Agent *) person)->handle << ".  His rating is incredibly high, suggesting a long "
				 "history of computer crime.  Police have had a file on this many for several years but have finally "
				 "caught him red handed.";

		part3 << "Due to the nature of his history, it is believed the prosecution may push for a life sentence for "
				 "this man.  The courts are becoming increasingly hostile to hackers and they may decide to deal a "
				 "heavy punishment to this hacker as a warning to others.";

	}


	//
	// Concatenate each part together and post it
	//

	headline << '\x0';
	part1 << '\x0';
	part2 << '\x0';
	part3 << '\x0';

	std::ostrstream details;
	details << part1.str () << "\n\n" << part2.str () << "\n\n" << part3.str () << '\x0';

	News *news = new News ();
	news->SetHeadline ( headline.str () );
	news->SetDetails ( details.str () );
	news->SetData ( NEWS_TYPE_ARREST, person->name, comp ? comp->ip : NULL );

	headline.rdbuf()->freeze( 0 );
	part1.rdbuf()->freeze( 0 );
	part2.rdbuf()->freeze( 0 );
	part3.rdbuf()->freeze( 0 );
	details.rdbuf()->freeze( 0 );
	//delete [] headline.str ();
	//delete [] part1.str ();
	//delete [] part2.str ();
	//delete [] part3.str ();
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}
