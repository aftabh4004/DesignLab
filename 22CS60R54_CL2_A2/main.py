import os
import sys
import json

org_err = sys.stderr

def pagedownload(link, file):
    from urllib.request import Request, urlopen
    req = Request('https://en.wikipedia.org/' + link, headers={'User-Agent': 'Mozilla/5.0'})

    webpage = urlopen(req).read()
    mydata = webpage.decode("utf8")
    f = open(file, 'w', encoding="utf-8")
    f.write(mydata)
    f.close()


def print_match_detail(match_data):
    result = ''
    if match_data['score'][0] > match_data['score'][1]:
        result = match_data['team1'] + ' won'
    elif match_data['score'][0] < match_data['score'][1]:
        result = match_data['team2'] + ' won'
    else:
        result = 'Draw'
    print('==============================================================================================')
    print()
    print('%30s\n' % (match_data['date']))
    print('%-20s %d-%d %20s' % (match_data['team1'], match_data['score'][0], match_data['score'][1], match_data['team2']))

    print('\n%28s' % "Goal Scorer")
    for i in range(max(len(match_data['goal_scorer_t1']), len(match_data['goal_scorer_t2']))):
        p1 = ''
        p2 = ''

        if i < len(match_data['goal_scorer_t1']):
            p1 = match_data['goal_scorer_t1'][i]
        
        if i < len(match_data['goal_scorer_t2']):
            p2 = match_data['goal_scorer_t2'][i]
        print('%20s     %-20s' % (p1, p2))
    

    if 'penalties_score' in match_data.keys() and match_data['penalties_score'] is not None:
        [ps1, ps2] = match_data['penalties_score'].split()
        if int(ps1) > int(ps2):
            result = match_data['team1'] + ' won'
        else:
            result = match_data['team2'] + ' won'
        print('\n%28s' % "Penalties")
        print('%24s\n' % match_data['penalties_score'])
        for i in range(max(len(match_data['pen_att_t1']), len(match_data['pen_att_t2']))):
            p1 = ''
            p2 = ''

            if i < len(match_data['pen_att_t1']):
                p1 = match_data['pen_att_t1'][i]
            
            if i < len(match_data['pen_att_t2']):
                p2 = match_data['pen_att_t2'][i]
            print('%20s     %-20s' % (p1, p2))
        


    print()
    print('%-20s: %s' % ('Stadium', match_data['stadium']))
    print('%-20s:%s' % ('Attendence', match_data['Attendence']))
    print('%-20s: %s' % ('Referee', match_data['Referee']))
    print('%-20s: %s' % ('Result', result))
    
    print()
    print('==============================================================================================')

def print_knockout_md(match_data, fp):
    if len(match_data) == 1:
        print_match_detail(match_data[0])
        fp.write(json.dumps(match_data[0]) + '\n')
        input('Press Any Key...')
        return

    print()
    print("Following matches were played.\n")
    i = 1
    for match in match_data:
        print('%d %s Vs %s' % (i, match['team1'], match['team2']))
        i = i + 1
    flag = False
    while not flag:
        try:
            mno = int(input('Enter match number: '))
        except:
            mno = -1
        if mno > len(match_data) or mno <= 0:
            print('Invalid input, Try again!')
        else:
            print_match_detail(match_data[mno-1])
            fp.write(json.dumps(match_data[mno-1]) + '\n')
            flag = True
            input('Press Any Key...')


def main():
    try:
        fp = open('log.txt', 'w')
        dump = open('dump.txt', 'w')
        sys.stderr = dump
    except:
        print('Unable to open files')
    ip = ''
    while True:
        os.system("clear")
        print("FIFA 2022 World Cup: Main menu")
        print("1. All Teams")
        print("2. Venue Details")
        print("3. Match Details: Group Stage")
        print("4. Match Details: Knockout Stage")
        print("5. Awards")
        print("6. Squad details for a team")
        print("0. Exit")

        try:
            choice = int(input())
        except:
            choice = -1

        if choice == -1:
            continue
        
        elif choice == 0:
            break

        elif choice == 1:
            ip = ip + '1. All Teams > '
            fp.write(ip + '\n')
            ip = ''
            print('Please wait...')
            from teamname import teams_data
            os.system("clear")
            print('Following teams participated in the tournament.\n')
            i = 1
            for team in teams_data.keys():
                fp.write(str(i) + '. ' +  team + '\n')
                print(str(i) + '. ' +  team)
                i += 1
            
            fp.write('\n')
            print()
            print("Press Any key...")
            input()

        elif choice == 2:
            ip = ip + '2. Venue Details > '
            fp.write(ip + '\n')
            ip = ''
            print('Please Wait...')
            from venue_details import venue_details
            os.system("clear")
            print('%-40s %s' % ('Stadium', 'Capacity'))
            fp.write('%-40s %s\n' % ('Stadium', 'Capacity'))
            for venue in venue_details:
                print('%-40s %s' % (venue['stadium'], venue['capacity']))
                fp.write('%-40s %s\n' % (venue['stadium'], venue['capacity']))

            
            print()
            print("Press Any key...")
            input()

        elif choice == 3:
            ip = ip + '3. Match Details: Group Stage > '
            flag_c3 = False
            while not flag_c3:
                i = 1
                groups = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
                print('Select group')
                for group in groups:
                    print('%d Group %c' % (i, group))
                    i = i + 1
                try:
                    gno = int(input('Enter group number: '))
                except:
                    gno = -1
                print("Please Wait...")
                if gno == 1:
                    ip += 'Group A > '
                    from groupA import match_data, goalf_c
                elif gno == 2:
                    ip += 'Group B > '
                    from groupB import match_data, goalf_c
                elif gno == 3:
                    ip += 'Group C > '
                    from groupC import match_data, goalf_c
                elif gno == 4:
                    ip += 'Group D > '
                    from groupD import match_data, goalf_c
                elif gno == 5:
                    ip += 'Group E > '
                    from groupE import match_data, goalf_c
                elif gno == 6:
                    ip += 'Group F > '
                    from groupF import match_data, goalf_c
                elif gno == 7:
                    ip += 'Group G > '
                    from groupG import match_data, goalf_c
                elif gno == 8:
                    ip += 'Group H > '
                    from groupH import match_data, goalf_c
                else:
                    print('Invalid group no., Try again!')
                    continue
                
                

                while True: 
                    os.system('clear')
                    print("Following matches were played.")
                    i = 1
                    for match in match_data:
                        print('%d %s Vs %s' % (i, match['team1'], match['team2']))
                        i = i + 1
                    print()
                    print('Teams qualified for knockout stage: %s and %s' % (goalf_c[0][0], goalf_c[1][0]))
                    print()
                    print("Want to know more about...")
                    print('1. Particular match details')
                    print('2. Goal forwarded and conceded for a team')
                    print('3. Go to previous menu')
                    print('0. Go to main menu')

                    
                    try:
                        subchoice = int(input())
                    except:
                        subchoice = -1

                    if subchoice == 0:
                        ip += '0. Go to main menu > ' 
                        flag_c3 = True
                        break

                    elif subchoice == 3:
                        ip += '3. Go to previous menu > '
                        os.system('clear')
                        break

                    elif subchoice == 1:
                        ip += '1. Particular match details > '
                        flag = False
                        while not flag:
                            try:
                                mno = int(input('Enter match number: '))
                            except:
                                mno = -1
                            if mno > len(match_data) or mno <= 0:
                                print('Invalid input, Try again!')
                            else:
                                print_match_detail(match_data[mno-1])
                                fp.write(ip + '\n')
                                ip = ''
                                fp.write(json.dumps(match_data[mno-1]) + '\n')
                                flag = True
                                input('Press Any Key')
                    

                    elif subchoice == 2:
                        ip += '2. Goal forwarded and conceded for a team > '
                        flag = False
                        while not flag:
                            team_choice = input('Enter team name: ')
                            ip += team_choice + ' > '
                            print()
                            for row in goalf_c:
                                if row[0].lower() == team_choice.lower():
                                    flag = True
                                    print('%-20s : %s' % ('Team: ', row[0]))
                                    print('%-20s : %s' % ('Goal forwarded: ',row[1]))
                                    print('%-20s : %s' % ('Goal conceded: ', row[2]))

                                    fp.write(ip + '\n')
                                    ip = ''
                                    fp.write(json.dumps(row) + '\n')
                                    input('Press Any key...')
                                    break
                            if not flag:
                                print('Invalid Team name, Try again!')
                    else:
                        input('Invalid input, Try again!\nPress Any key...')
                        


            
        elif choice == 4:
            ip = ip + '4. Match Details: Knockout Stage > '
            print('Please wait...')
            from roundof16 import match_data as r16m
            from quarterfinals import match_data as qfm
            from semifinals import match_data as sfm
            from tppo import match_data as tppom
            from final import match_data as fnm
            while True:
                os.system('clear')
                print('Fixtures\n')
                print('%-35s%-35s%-35s%-35s%-35s' % ('Round of 16', 'Quarter-Finals', 'Semi-Finals', 'Third place play-off', 'Final'))
                print()
                for i in range(8):
                    r16, qf, sf, tppo, fn = ('', '', '', '', '')
                    if i < len(r16m):
                        r16 = '%-13s vs %-13s'% (r16m[i]['team1'], r16m[i]['team2']) 
                    if i < len(qfm):
                        qf = '%-13s vs %-13s'% (qfm[i]['team1'], qfm[i]['team2'])
                    if i < len(sfm):
                        sf = '%-13s vs %-13s'% (sfm[i]['team1'], sfm[i]['team2'])
                    if i < len(tppom):
                        tppo = '%-13s vs %-13s'% (tppom[i]['team1'], tppom[i]['team2'])
                    if i < len(fnm):
                        fn = '%-13s vs %-13s'% (fnm[i]['team1'], fnm[i]['team2'])
                    print('%-35s%-35s%-35s%-35s%-35s' % (r16, qf, sf, tppo, fn))

                
                
                print()
                print('Want to know more about...')
                print('1. Particular match details')
                print('2. Match detail given two team name')
                print('0. Go to main menu')

                try:
                    subchoice = int(input())
                except:
                    subchoice = -1

                if subchoice == 0:
                    ip += '0. Go to main menu > '
                    break
                elif subchoice == 1:
                    ip += '1. Particular match details > '
                    while True:
                        os.system('clear')
                        print('Fixtures\n')
                        print('%-35s%-35s%-35s%-35s%-35s' % ('Round of 16', 'Quarter-Finals', 'Semi-Finals', 'Third place play-off', 'Final'))
                        print()
                        for i in range(8):
                            r16, qf, sf, tppo, fn = ('', '', '', '', '')
                            if i < len(r16m):
                                r16 = '%-13s vs %-13s'% (r16m[i]['team1'], r16m[i]['team2']) 
                            if i < len(qfm):
                                qf = '%-13s vs %-13s'% (qfm[i]['team1'], qfm[i]['team2'])
                            if i < len(sfm):
                                sf = '%-13s vs %-13s'% (sfm[i]['team1'], sfm[i]['team2'])
                            if i < len(tppom):
                                tppo = '%-13s vs %-13s'% (tppom[i]['team1'], tppom[i]['team2'])
                            if i < len(fnm):
                                fn = '%-13s vs %-13s'% (fnm[i]['team1'], fnm[i]['team2'])
                            print('%-35s%-35s%-35s%-35s%-35s' % (r16, qf, sf, tppo, fn))
                        
                        print()
                        print('Following are the knockout stages.\n')
                        print('1. Round of 16')
                        print('2. Quarter-Finals')
                        print('3. Semi-Finals')
                        print('4. Third place play-off')
                        print('5. Final')
                        print('0. Go to previous menu')
                        
                        try:
                            subsubchoice = int(input('Enter choice: '))
                        except:
                            subsubchoice = -1

                        if subsubchoice == 0:
                            ip += '0. Go to previous menu > '
                            break
                        elif subsubchoice == 1:
                            ip += 'Round 16 > '
                            fp.write(ip + '\n')
                            ip = ''
                            print_knockout_md(r16m, fp)
                        elif subsubchoice == 2:
                            ip += 'Quarter-finals > '
                            fp.write(ip + '\n')
                            ip = ''
                            print_knockout_md(qfm, fp)
                        elif subsubchoice == 3:
                            ip += 'Semi-Finals > '
                            fp.write(ip + '\n')
                            ip = ''
                            print_knockout_md(sfm, fp)
                        elif subsubchoice == 4:
                            ip += 'Third place play-off > '
                            fp.write(ip + '\n')
                            ip = ''
                            print_knockout_md(tppom, fp)
                        elif subsubchoice == 5:
                            ip += 'final > '
                            fp.write(ip + '\n')
                            ip = ''
                            print_knockout_md(fnm, fp)
                        else:
                            print('Invalid input, Try again!')
                            input('Press Any key...')
                
                elif subchoice == 2:
                    t1 = input('Team 1: ').lower()
                    t2 = input('Team 2: ').lower()
                    ip += 'team1 ' + t1 +  ' > '
                    ip += 'team2 ' + t2 +  ' > '
                    isMatch = False
                    for stage in [r16m, qfm, sfm, tppom, fnm]:
                        for match in stage:
                            if t1 == match['team1'].lower() and t2 == match['team2'].lower() or t1 == match['team2'].lower() and t2 == match['team1'].lower():
                                isMatch = True
                                fp.write(ip + '\n')
                                ip = ''
                                fp.write(json.dumps(match) + '\n')
                                print_match_detail(match)
                                input('Press Any key...')
                                break
                    if not isMatch:
                        print('No matches were played between %s and %s' % (t1, t2))
                        input('Press Any key...')

                else:
                    input('Invalid input, Try again!\nPress Any key...')
                        


                
        elif choice == 5:
            ip += '5. Awards > '
            print('Please wait...')
            from awards import award_data
            os.system("clear")
            print('%-40s %s' % ('Awards', 'Awardee'))
            fp.write('%-40s %s\n' % ('Awards', 'Awardee'))

            for award in award_data:
                print('%-40s %s' % (award, award_data[award]))
                fp.write('%-40s %s\n' % (award, award_data[award]))

            
            print()
            print("Press Any key...")
            input()

        elif choice == 6:
            ip += '6. Squad details for a team > '
            print('Please wait...')
            from teamname import teams_data
            team_name = input('Enter team name: ')

            for team in teams_data.keys():
                if team.lower() == team_name.lower():
                    pagedownload(teams_data[team], team + '_data.html')
                    from squad import main as squad_main
                    squad_main(team + '_data.html')
                    input('Press Any key...')


        else:
            input('Invalid input, Try again!.\nPress Any key...')
    sys.stderr = org_err
    fp.close()
    dump.close()
        

if __name__ == "__main__":
    main()