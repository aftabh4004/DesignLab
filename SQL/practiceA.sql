-- PART A

-- 1. List the names of all the players in the database

select Player.Name from Player;

-- 2. List the names of all players who have ever been assigned a seed for any tournament (doubles or singles).

select distinct p.Name
from Player p, PlayedIn pi, Registration r
where pi.Seed is not NULL
and pi.RegistrNum = r.RegistrNum
and p.PID = r.PID;

-- 3. List the pairs of players who played doubles tennis at Wimbledon in 2007. 
--    Do not include duplicate entries (ex: 〈personA, personB〉 as well as 〈personB, personA〉).

select p1.Name, p2.Name
from Player p1, Player p2, PlayedIn pi, Registration r1, Registration r2, Tournament t
where t.Name = 'Wimbledon' 
and t.TType = 'Doubles'
and pi.TID = t.TID
and pi.RegistrNum = r1.RegistrNum
and pi.RegistrNum = r2.RegistrNum
and p1.PID = r1.PID
and p2.PID = r2.PID
and p1.PID > p2.PID;


select p1.Name, p2.Name
from Player p1, Player p2, PlayedIn pi, Registration r1, Registration r2, Tournament t
where p1.PID = r1.PID
and p2.PID = r2.PID
and p1.PID < p2.PID
and r1.RegistrNum = r2.RegistrNum
and r1.PID != r2.PID
and pi.RegistrNum = r1.RegistrNum
and pi.TID = t.TID
and t.Name = 'Wimbledon' 
and t.TType = 'Doubles';




-- 4. List the names of all players who have lost to Roger Federer in the finals of any tourna-
--    ment, as well as the name of the tournament they lost in. Include results only for singles
--    tennis.


with RogerRN(RegistrNum) as (
                                select r.RegistrNum
                                from Player p, Registration r
                                where p.Name = 'Roger Federer'
                                and p.PID = r.PID
                            ),
    LoserPID(lPID, TName) as (
                                select r.PID, t.Name
                                from Tournament t, Matches m, MatchResults mr, Registration r
                                where t.TID = m.TID
                                and t.TType = 'Singles'
                                and t.NumRounds = m.Round
                                and m.MID = mr.MID
                                and (
                                        (m.RegistrNum2 in (select RogerRN.RegistrNum from RogerRN)
                                        and m.RegistrNum2 = mr.Winner
                                        and r.RegistrNum = m.RegistrNum1)
                                        or
                                        (m.RegistrNum1 in (select RogerRN.RegistrNum from RogerRN)
                                        and m.RegistrNum1 = mr.Winner
                                        and r.RegistrNum = m.RegistrNum2)
                                    )
                            )
select p.Name, l.TName
from Player p, LoserPID l
where p.PID = l.lPID;





-- Alternate
select p.Name, t.Name
from Registration rroger, Registration r, Matches m, MatchResults mr,(select p.PID PID
                                                                    from Player p
                                                                    where name='Roger Federer') roger, 
                                                   Tournament t, Player p
where rroger.PID = roger.PID 
AND m.MID = mr.MID 
AND mr.Winner = rroger.RegistrNum 
AND(
        (rroger.RegistrNum = m.RegistrNum1 AND r.RegistrNum = m.RegistrNum2)
        OR  
        (rroger.RegistrNum = m.RegistrNum2 AND r.RegistrNum = m.RegistrNum1)
    )
AND t.TID = m.TID 
AND t.NumRounds = m.Round 
AND t.TType = 'Singles' 
AND p.PID = r.PID;



-- 5. For all final round single matches, list the winner and loser of matches that were be-
--    tween two seeded players, as well as their seeds. Modify the titles of the columns to be
--    something useful, like WinnerName, WinnerSeed, LoserName, LoserSeed.

select p1.Name as WinnerName, pi1.Seed as WinnerSeed, p2.Name as LoserName, pi2.Seed as LoserSeed
from Player p1, Player p2, PlayedIn pi1, PlayedIn pi2, Registration r1, Registration r2, Tournament t, Matches m, MatchResults mr
where t.TType = 'Singles'
and t.TID = m.TID
and t.NumRounds = m.Round
and m.MID = mr.MID
and m.RegistrNum1 = pi1.RegistrNum
and m.RegistrNum2 = pi2.RegistrNum
and r1.PID = p1.PID
and r2.PID = p2.PID
and pi1.Seed is not NULL
and pi2.Seed is not NULL
and (
        (
            m.RegistrNum1 = mr.Winner
            and m.RegistrNum1 = r1.RegistrNum
            and m.RegistrNum2 = r2.RegistrNum
        )
        or
        (
            m.RegistrNum2 = mr.Winner
            and m.RegistrNum2 = r1.RegistrNum
            and m.RegistrNum1 = r2.RegistrNum
        )
    );


-- Alternate

select winner.playername winnername, winner.Seed winnerSeed,
loser.playername losername, loser.Seed loserSeed
from Matches m, Tournament t, MatchResults mr,
(select r.RegistrNum, pi.Seed Seed, p.Name playerName
from Player p, Registration r, PlayedIn pi
where p.PID = r.PID AND r.RegistrNum = pi.RegistrNum AND
pi.Seed is not null) winner,
(select r.RegistrNum, pi.Seed Seed, p.Name playerName
from Player p, Registration r, PlayedIn pi
where p.PID = r.PID AND r.RegistrNum = pi.RegistrNum AND
pi.Seed is not null) loser
where m.TID = t.TID AND t.ttype = 'Singles' AND m.Round = t.NumRounds AND
m.MID = mr.MID AND
(m.RegistrNum1 = winner.RegistrNum OR m.RegistrNum2 = winner.RegistrNum) AND
(m.RegistrNum1 = loser.RegistrNum OR m.RegistrNum2 = loser.RegistrNum) AND
winner.RegistrNum != loser.RegistrNum AND mr.Winner = winner.RegistrNum



-- 6. List the names of all US players who have participated in at least two tournaments in
--    2007. Do not use any aggregating functions for this problem.

with USPlayer(usPID) as (
                            select p.PID
                            from Player p
                            where p.CCode = 'USA'
                        ) 
select distinct p.Name
from USPlayer us, Registration r, Player p
where us.usPID = r.PID
and r.PID = p.PID
group by p.PID, p.Name
having Count(*) >= 2;


-- Alternate
select distinct usplayers.name
from Tournament t1, Tournament t2, Registration r1, Registration r2,
PlayedIn pi1, PlayedIn pi2,
(select p.PID PID, p.Name
from Player p, CountryCodes cc
where p.CCode = cc.Code AND cc.Country = 'United States') usplayers
where t1.TID = pi1.TID AND pi1.RegistrNum = r1.RegistrNum AND
t2.TID = pi2.TID AND pi2.RegistrNum = r2.RegistrNum AND
r1.PID = usplayers.PID AND r2.PID = usplayers.PID AND
r1.RegistrNum != r2.RegistrNum
and STR_TO_DATE(t1.StartDate,'%d-%M-%y') >= STR_TO_DATE('01-01-2007','%d-%m-%y') 
and STR_TO_DATE(t2.StartDate,'%d-%M-%y') >= STR_TO_DATE('01-01-2007','%d-%m-%y');


-- 11. List the names, tournament types, and lengths (in days) of all tournaments that were longer
--     than one week

select t.Name, t.TType, STR_TO_DATE(t.EndDate, '%d-%M-%y') - STR_TO_DATE(t.StartDate, '%d-%M-%y') as Duration
from Tournament t
where  STR_TO_DATE(t.EndDate, '%d-%M-%y') - STR_TO_DATE(t.StartDate, '%d-%M-%y') > 7;


-- 12. List the names of all male German players who registered for the 2007 Australian Open
--     singles.

with GermenPlayers(gPID, gName) as (
                                select p.PID, p.Name
                                from Player p, CountryCodes cc
                                where p.CCode = cc.Code
                                and p.Gender = 'M'
                                and cc.Country = 'Germany' 
                            )
select distinct gp.gName
from Tournament t, PlayedIn pi, Registration r, GermenPlayers gp
where t.Name = 'Australian Open'
and t.TType = 'Singles'
and t.TID = pi.TID
and pi.RegistrNum = r.RegistrNum
and r.PID = gp.gPID;


-- PART B


-- 1. For all singles quarterfinal, semifinal, and final round matches that only took 3 sets, list
--    the Tournament Name, Year, Winner of the Match, and the Match score (For the match
--    score, print two columns per set, displaying the number of games each player won. Call these
--    columns something meaningful, and ignore any tiebreaker results.).


select t.Name, t.StartDate as Year, p.Name, s1.WinnerGames, s1.LoserGames, s2.WinnerGames, s2.LoserGames, s3.WinnerGames, s3.LoserGames 
from Tournament t, Matches m, MatchResults mr, Registration r, Player p, SetScore s1, SetScore s2, SetScore s3 
where t.TType = 'Singles'
and t.TID = m.TID
and m.Round >= (t.NumRounds - 2)
and m.MID = mr.MID
and mr.NumSets = 3
and mr.Winner = r.RegistrNum
and r.PID = p.PID
and mr.MID = s1.MID
and s1.SetNum = 1
and mr.MID = s2.MID
and s2.SetNum = 2
and mr.MID = s3.MID
and s3.SetNum = 3;
