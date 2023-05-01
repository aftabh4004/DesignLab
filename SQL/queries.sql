select Name, StartDate, EndDate, TType, NumRounds
from Tournament
where NumRounds > 5;

select p.Name
from Player p, PlayedIn pi, Registration r
where pi.Seed is not NULL
and pi.RegistrNum = r.RegistrNum
and r.PID = p.PID;


select distinct p.Name
from Player p, Tournament t, Matches m, Registration r
where t.TType = 'Doubles'
and t.Name = 'Wimbledon'
and t.TID = m.TID
and (m.RegistrNum1 = r.RegistrNum or m.RegistrNum2 = r.RegistrNum)
and r.PID = p.PID; 





select distinct p1.Name, p2.Name
from Player p1, Player p2, Tournament t, PlayedIn pi, Registration r1, Registration r2
where t.TType = 'Doubles'
and t.Name = 'Wimbledon'
and t.TID = pi.TID
and pi.RegistrNum = r1.RegistrNum
and pi.RegistrNum = r2.RegistrNum
and r1.PID = p1.PID
and r2.PID = p2.PID
and p1.PID > p2.PID;


select sum(NumSets)
from MatchResults;

-- Assignment problems

select p.Name
from Player p, CountryCodes c
where c.Code = p.CCode
and c.Country = 'Australia';

select distinct p.Name
from Player p, Tournament t, Matches m, Registration r
where t.TID = m.TID
and t.TType = 'Singles'
and m.Round >= (t.NumRounds - 2)
and (m.RegistrNum1 = r.RegistrNum or m.RegistrNum2 = r.RegistrNum)
and r.PID = p.PID;

select m.MID, p.Name
from Player p, Tournament t, MatchResults mr, Matches m, Registration r
where t.Name = 'US Open'
and t.TID = m.TID
and m.MID = mr.MID
and mr.Winner = r.RegistrNum
and r.PID = p.PID;


select p.Name, t.Name
from Player p, Tournament t, MatchResults mr, Matches m, Registration r
where t.TType = 'Singles'
and t.TID = m.TID
and t.NumRounds = m.Round
and m.MID = mr.MID
and mr.Winner = r.RegistrNum
and r.PID = p.PID;



select pp.Name
from Player pp, Registration r
where pp.CCode IN ('RUS', 'USA', 'CHI')
and pp.PID = r.PID
and r.RegistrNum IN (
                select m.RegistrNum1
                from Matches m
                where m.RegistrNum2 IN (
                                        select rr.RegistrNum
                                        from Player p, Registration rr
                                        where rr.PID = p.PID
                                        and p.Name = 'Tommy Haas'
                                        )
                UNION
                select m.RegistrNum2
                from Matches m
                where m.RegistrNum1 IN (
                                        select rr.RegistrNum
                                        from Player p, Registration rr
                                        where rr.PID = p.PID
                                        and p.Name = 'Tommy Haas'
                                        )

            );


with tommyRN(RegistrNum) as (
                                select rr.RegistrNum
                                from Player p, Registration rr
                                where rr.PID = p.PID
                                and p.Name = 'Tommy Haas'
                            ), 

    opp(RegistrNum) as (
                            select m.RegistrNum1
                            from Matches m, tommyRN
                            where m.RegistrNum2 = tommyRN.RegistrNum
                            UNION
                            select m.RegistrNum2
                            from Matches m, tommyRN
                            where m.RegistrNum1 = tommyRN.RegistrNum
                        )
select pp.Name
from Player pp, Registration r, opp
where pp.CCode IN ('RUS', 'USA', 'CHI')
and pp.PID = r.PID
and r.RegistrNum = opp.RegistrNum;