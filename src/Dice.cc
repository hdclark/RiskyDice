//Dice.cc

#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>    
#include <vector>
//#include <cfenv>              //Needed for std::feclearexcept(FE_ALL_EXCEPT).

#include <cstdlib>            //Needed for exit() calls.
#include <utility>            //Needed for std::pair.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>            //Needed for exit() calls.
#include <exception>
#include <experimental/any>
#include <experimental/optional>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>    
#include <tuple>
#include <type_traits>
#include <utility>            //Needed for std::pair.
#include <vector>
#include <random>
#include <iomanip>

#include <SFML/Graphics.hpp>

int main(int, char**){
    // Setup the RNG.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 6);

    // Setup the display window.
    sf::RenderWindow window;
    window.create(sf::VideoMode(640, 480), "Risky Dice");
    window.setFramerateLimit(60);

    //Attempt to load fonts. We should try a few different files, and include a back-up somewhere accessible...
    sf::Font afont;
    if( !afont.loadFromFile("/usr/share/fonts/TTF/cmr10.ttf")
    &&  !afont.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMono.ttf") ){
        throw std::runtime_error("Unable to find a suitable font file");
    }

    std::array<sf::Text,6> text_arr;
    for(auto &t : text_arr){
        t.setFont(afont);
        t.setString("");
        t.setCharacterSize(25); //Size in pixels, not in points.
        t.setFillColor(sf::Color::Green);
        t.setOutlineColor(sf::Color::Green);
    }

    const auto usage = []() -> std::string {
        std::stringstream ss;
        ss << "Press:" << std::endl;
        ss << "-------------------------------------------------" << std::endl;
        ss << "' ' to simulate single attack dice rolling." << std::endl;
        ss << "'s' to estimate single attack losses." << std::endl;
        ss << "'XvY' (e.g., '10v5') for whole battle simulation." << std::endl;
        ss << "'enter'/'return' to launch battle simulation." << std::endl;
        ss << "'c' to clear battle simulation input." << std::endl;
        ss << "'ESC' to exit program." << std::endl;
        return ss.str();
    };
    text_arr[2].setString(usage());


    std::string freeform; // Free-form attacker vs defender battle input holder.

/*
    //Create some primitive shapes, textures, and text objects for display later.
    sf::CircleShape smallcirc(10.0f);
    smallcirc.setFillColor(sf::Color::Transparent);
    smallcirc.setOutlineColor(sf::Color::Green);
    smallcirc.setOutlineThickness(1.0f);

    bool drawcursortext = false; //Usually gets in the way. Sort of a debug feature...
    sf::Text cursortext;
    cursortext.setFont(afont);
    cursortext.setString("");
    cursortext.setCharacterSize(15); //Size in pixels, not in points.
    cursortext.setFillColor(sf::Color::Green);
    cursortext.setOutlineColor(sf::Color::Green);

    sf::Text BRcornertext;
    std::stringstream BRcornertextss;
    BRcornertext.setFont(afont);
    BRcornertext.setString("");
    BRcornertext.setCharacterSize(9); //Size in pixels, not in points.
    BRcornertext.setFillColor(sf::Color::Red);
    BRcornertext.setOutlineColor(sf::Color::Red);

    sf::Text BLcornertext;
    std::stringstream BLcornertextss;
    BLcornertext.setFont(afont);
    BLcornertext.setString("");
    BLcornertext.setCharacterSize(15); //Size in pixels, not in points.
    BLcornertext.setFillColor(sf::Color::Blue);
    BLcornertext.setOutlineColor(sf::Color::Blue);

            sf::Text contourtext;
            std::stringstream contourtextss;
            contourtext.setFont(afont);
            contourtext.setString("");
            contourtext.setCharacterSize(12); //Size in pixels, not in points.
            contourtext.setFillColor(sf::Color::Green);
            contourtext.setOutlineColor(sf::Color::Green);

    const sf::Color NaN_Color(60,0,0); //Dark red. Should not be very distracting.
    const sf::Color Pos_Contour_Color(sf::Color::Blue);
    const sf::Color Neg_Contour_Color(sf::Color::Red);
    const sf::Color Editing_Contour_Color(255,121,0); //"Vivid Orange."
*/


    // This routine returns the remaining number of units in each army.
    // The attacker gets N_a dice (3, 2, or 1) and the defender gets N_d dice (2 or 1).
    auto sim_losses = [&](int N_a, int N_d) -> std::pair<int, int> { 

        std::vector<int> a; // Attacker rolls.
        std::vector<int> d; // Defender rolls.
        for(size_t i = 0; i < N_a; ++i) a.emplace_back(dis(gen));
        for(size_t i = 0; i < N_d; ++i) d.emplace_back(dis(gen));

        // Sort and thus pair-off the attacker and defender dice.
        std::sort( std::begin(a), std::end(a), std::greater<int>() );
        std::sort( std::begin(d), std::end(d), std::greater<int>() );

        int loss_a = 0;
        int loss_d = 0;
        if(a.at(0) > d.at(0)){
            ++loss_d;
        }else{
            ++loss_a;
        }
        if( (a.size() >= 2) 
        &&  (d.size() >= 2) ){
            if(a[1] > d[1] ){
                ++loss_d;
            }else{
                ++loss_a;
            }
        }

        return std::make_pair<int, int>( N_a - loss_a, N_d - loss_d );
    };


    // Simulates attacking with N_a units against N_d defending units.
    // Note that your army must leave at least one unit behind. This routine ignores the unit and it
    // does NOT count toward the attacking army size.
    auto sim_battle = [&](int N_a, int N_d) -> std::string {
        const auto N_a_0 = N_a;
        const auto N_d_0 = N_d;

        while( (N_a > 0) && (N_d > 0) ){

            // Determine the number of units that will participate in the next battle.
            const auto B_a = std::min( N_a, 3 );
            const auto B_d = std::min( N_d, 3 );

            // Subtract these units from the total army sizes.
            N_a -= B_a;
            N_d -= B_d;

            // Simulate the number of units remaining after the battle.
            const auto r = sim_losses(B_a, B_d);

            // Restore the units that returned.
            N_a += r.first;
            N_d += r.second;
        }

        std::stringstream ss;

        ss << N_a_0 << " attacking " << N_d_0 << std::endl;
        ss << "-----------------" << std::endl;

        const auto actual_lost_a = N_a_0 - N_a;
        const auto actual_lost_d = N_d_0 - N_d;
        const auto a_won = (N_d == 0);

        ss << "Attacker loses: " << actual_lost_a << std::endl;
        ss << "Defender loses: " << actual_lost_d << std::endl;
        ss << std::endl;
        ss << ( (a_won) ? "Attacker" : "Defender" ) << " wins." << std::endl;
        ss << ( (a_won) ? N_a : N_d ) << " units remain." << std::endl;

        return ss.str();
    };

    // Estimates expected outcome from attacking with N_a units against N_d defending units.
    // Note that your army must leave at least one unit behind. This routine ignores the unit and it
    // does NOT count toward the attacking army size.
    auto est_battle_losses = [&](int N_a, int N_d) -> std::string {
        std::stringstream ss;

        ss << N_a << " attacking " << N_d << std::endl;
        ss << "-----------------" << std::endl;

        size_t wins_a = 0; // Number of times won, in total.
        size_t wins_d = 0;

        size_t loss_a = 0; // Number of lost units, in total.
        size_t loss_d = 0;

        size_t N_sims = 10000;

        for(size_t j = 0; j < N_sims; ++j){
            const auto N_a_0 = N_a;
            const auto N_d_0 = N_d;

            while( (N_a > 0) && (N_d > 0) ){

                // Determine the number of units that will participate in the next battle.
                const auto B_a = std::min( N_a, 3 );
                const auto B_d = std::min( N_d, 3 );

                // Subtract these units from the total army sizes.
                N_a -= B_a;
                N_d -= B_d;

                // Simulate the number of units remaining after the battle.
                const auto r = sim_losses(B_a, B_d);

                // Restore the units that returned.
                N_a += r.first;
                N_d += r.second;
            }

            const auto actual_lost_a = N_a_0 - N_a;
            const auto actual_lost_d = N_d_0 - N_d;
            const auto a_won = (N_d == 0);

            wins_a += (a_won) ? 1 : 0; 
            wins_d += (a_won) ? 0 : 1; 

            loss_a += actual_lost_a;
            loss_d += actual_lost_d;

            // Reset the initial numbers for the next simulation.
            N_a = N_a_0;
            N_d = N_d_0;
        }

        const auto expected_loss_a = static_cast<float>(loss_a) / static_cast<float>(N_sims);
        const auto expected_loss_d = static_cast<float>(loss_d) / static_cast<float>(N_sims);
        const auto a_win_ratio = static_cast<float>(wins_a) / static_cast<float>(N_sims);
        const auto d_win_ratio = static_cast<float>(wins_d) / static_cast<float>(N_sims);

        ss << "Expected losses" << std::endl;
        ss << "  (" << N_sims << " sims)" << std::endl;
        ss << std::endl;
        if(a_win_ratio > d_win_ratio){
            ss << "Attacker wins (" << std::setprecision(1) << std::fixed << a_win_ratio * 100.0 << "%)" << std::endl;
        }else{
            ss << "Defender wins (" << std::setprecision(1) << std::fixed << d_win_ratio * 100.0 << "%)" << std::endl;
        }
        ss << "Attacker loses: " << std::setprecision(2) << std::fixed << expected_loss_a << std::endl;
        ss << "Defender loses: " << std::setprecision(2) << std::fixed << expected_loss_d << std::endl;

        return ss.str();
    };


    //Run until the window is closed or the user wishes to exit.
    while(window.isOpen()){

        //Check if any events have accumulated since the last poll. If so, deal with them.
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
                break;
            }else if(window.hasFocus() && (event.type == sf::Event::KeyPressed)){

                // Close the window and thus terminate the program.
                if(false){
                }else if(event.key.code == sf::Keyboard::Escape){
                    window.close();
                    break;

                }

            }else if(window.hasFocus() && (event.type == sf::Event::TextEntered) 
                                       && (event.text.unicode < 128)){
                //Not the same as KeyPressed + KeyReleased. Think unicode characters, or control keys.
                const auto thechar = static_cast<char>(event.text.unicode);

                if( false ){

                // Close the window, which will terminate the program.
                }else if( (thechar == 'q') || (thechar == 'Q') ){
                    window.close();
                    return 0;

                // Simulate new dice rolls for a single attack round.
                }else if( thechar == ' ' ){

                    auto sim_rolls = [&](int N_a, int N_d) -> std::string {
                        // The attacker gets N_a dice (3, 2, or 1) and the defender gets N_d dice (2 or 1).

                        std::stringstream ss;

                        ss << N_a << " attacking " << N_d << std::endl;
                        ss << "-----------------" << std::endl;

                        std::vector<int> a; // Attacker rolls.
                        std::vector<int> d; // Defender rolls.
                        for(size_t i = 0; i < N_a; ++i) a.emplace_back(dis(gen));
                        for(size_t i = 0; i < N_d; ++i) d.emplace_back(dis(gen));

                        // Sort and thus pair-off the attacker and defender dice.
                        std::sort( std::begin(a), std::end(a), std::greater<int>() );
                        std::sort( std::begin(d), std::end(d), std::greater<int>() );

                        ss << "Attacker: ";
                        for(const auto &i : a) ss << i << " ";
                        ss << std::endl;
                        ss << "Defender: ";
                        for(const auto &i : d) ss << i << " ";
                        ss << std::endl;

                        int loss_a = 0;
                        int loss_d = 0;
                        if(a.at(0) > d.at(0)){
                            ++loss_d;
                        }else{
                            ++loss_a;
                        }
                        if( (a.size() >= 2) 
                        &&  (d.size() >= 2) ){
                            if(a[1] > d[1] ){
                                ++loss_d;
                            }else{
                                ++loss_a;
                            }
                        }
                        ss << std::endl;
                        ss << "Attacker loses: " << loss_a << std::endl;
                        ss << "Defender loses: " << loss_d << std::endl;

                        return ss.str();
                    };

                    text_arr.at(0).setString(sim_rolls(3,2));
                    text_arr.at(1).setString(sim_rolls(3,1));

                    text_arr.at(2).setString(sim_rolls(2,2));
                    text_arr.at(3).setString(sim_rolls(2,1));

                    text_arr.at(4).setString(sim_rolls(1,2));
                    text_arr.at(5).setString(sim_rolls(1,1));


                // Numerically estimate statistics for single attack rounds.
                }else if( thechar == 's' ){

                    auto est_expected_losses = [&](int N_a, int N_d) -> std::string {
                        // The attacker gets N_a dice (3, 2, or 1) and the defender gets N_d dice (2 or 1).

                        std::stringstream ss;

                        ss << N_a << " attacking " << N_d << std::endl;
                        ss << "-----------------" << std::endl;

                        size_t loss_a = 0;
                        size_t loss_d = 0;
                        size_t N_sims = 10000;

                        std::vector<int> a; // Attacker rolls.
                        std::vector<int> d; // Defender rolls.
                        for(size_t j = 0; j < N_sims; ++j){
                            a.clear();
                            d.clear();
                            for(size_t i = 0; i < N_a; ++i) a.emplace_back(dis(gen));
                            for(size_t i = 0; i < N_d; ++i) d.emplace_back(dis(gen));

                            // Sort and thus pair-off the attacker and defender dice.
                            std::sort( std::begin(a), std::end(a), std::greater<int>() );
                            std::sort( std::begin(d), std::end(d), std::greater<int>() );

                            if(a.at(0) > d.at(0)){
                                ++loss_d;
                            }else{
                                ++loss_a;
                            }
                            if( (a.size() >= 2) 
                            &&  (d.size() >= 2) ){
                                if(a[1] > d[1] ){
                                    ++loss_d;
                                }else{
                                    ++loss_a;
                                }
                            }
                        }

                        const auto expected_loss_a = static_cast<float>(loss_a) / static_cast<float>(N_sims);
                        const auto expected_loss_d = static_cast<float>(loss_d) / static_cast<float>(N_sims);

                        ss << "Expected losses" << std::endl;
                        ss << "  (" << N_sims << " sims)" << std::endl;
                        ss << std::endl;
                        ss << "Attacker loses: " << std::setprecision(2) << std::fixed << expected_loss_a << std::endl;
                        ss << "Defender loses: " << std::setprecision(2) << std::fixed << expected_loss_d << std::endl;

                        return ss.str();
                    };

                    text_arr.at(0).setString(est_expected_losses(3,2));
                    text_arr.at(1).setString(est_expected_losses(3,1));

                    text_arr.at(2).setString(est_expected_losses(2,2));
                    text_arr.at(3).setString(est_expected_losses(2,1));

                    text_arr.at(4).setString(est_expected_losses(1,2));
                    text_arr.at(5).setString(est_expected_losses(1,1));

                // Push the input onto the stack for later interpretation.
                //
                // Input should be of the form 'XvY' (for example, '10v53'), but input is not parsed until pressing
                // 'enter'/'return'.
                }else if( false 
                      ||  (thechar == '0')
                      ||  (thechar == '1')
                      ||  (thechar == '2')
                      ||  (thechar == '3')
                      ||  (thechar == '4')
                      ||  (thechar == '5')
                      ||  (thechar == '6')
                      ||  (thechar == '7')
                      ||  (thechar == '8')
                      ||  (thechar == '9')
                      ||  (thechar == 'v')
                      ||  (thechar == 'V') ){

                    freeform += thechar;
                    for(auto &t : text_arr){
                        t.setString(" ");
                    }
                    text_arr.at(2).setString(freeform);

                // Clear the freeform text.
                }else if( thechar == 'c' ){
                    freeform.clear();
                    for(auto &t : text_arr){
                        t.setString(" ");
                    }

                // Interpret the freeform text, if possible.
                }else if( (thechar == '\n')
                      ||  (thechar == '\v')
                      ||  (thechar == '\r') ){

                    // Simulate successive battles until one army is defeated.
                    try{
                        const auto N_a = std::stoi(freeform);

                        const auto v_pos = freeform.find_last_of("vV");
                        if(v_pos == std::string::npos) throw std::runtime_error("");
                        const auto post_v = freeform.substr(v_pos+1);

                        const auto N_d = std::stoi(post_v);

                        if( (N_a < 1) || (1000 < N_a)
                        ||  (N_d < 1) || (1000 < N_d) ){
                            throw std::runtime_error("");
                        }

                        for(auto &t : text_arr){
                            t.setString(" ");
                        }
                        text_arr.at(2).setString(sim_battle(N_a, N_d));
                        text_arr.at(3).setString(est_battle_losses(N_a, N_d));
                        freeform.clear();
                    }catch(const std::exception &){}

                // Fallback to a simplified usage screen.
                }else{
                    std::stringstream ss;
                    ss << "Pressed '" << thechar << "'" << std::endl;
                    ss << usage();
                    for(auto &t : text_arr){
                        t.setString(" ");
                    }
                    text_arr[2].setString(ss.str());
                }

            }else if(event.type == sf::Event::Resized){
                sf::View view;

                //Shrink the image depending on the amount of window space available. The image might disappear off the
                // screen if the window is too small, but nothing gets squished.
                view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
                window.setView(view);

            }else if(window.hasFocus() && (event.type == sf::Event::KeyReleased)){
            }else if(window.hasFocus() && (event.type == sf::Event::MouseMoved)){
            }else if(event.type == sf::Event::LostFocus){
            }else if(event.type == sf::Event::GainedFocus){
            }else if(event.type == sf::Event::MouseEntered){
            }else if(event.type == sf::Event::MouseLeft){
            }else{
                std::cerr << "Ignored event!" << std::endl;
            }

        }

        // -------------------------------------- Rendering ----------------------------------------

        //Begin drawing the window contents.
        window.clear(sf::Color::Black);

        // Move the given text (using the current text contents) across the screen using clamped coordinates
        // with x (horizontal) within [-1,1] and y (vertical) within [-1,1].

        auto shift = [&]( sf::Text &t, float horiz, float vert ) -> void {
            const auto item_bbox = t.getGlobalBounds();
            const auto item_shft = sf::Vector2f( item_bbox.left + (horiz - (-1.0)) * 0.5 * item_bbox.width, 
                                                 item_bbox.top  + (vert  - (-1.0)) * 0.5 * item_bbox.height );

            const auto wndw_view = window.getView();
            const auto view_cntr = wndw_view.getCenter();
            const auto view_size = wndw_view.getSize();
            const auto view_shft = sf::Vector2f( view_cntr.x + horiz * 0.45f * view_size.x, 
                                                 view_cntr.y - vert  * 0.40f * view_size.y );
     
            //We should have that the item's bottom right corner coincides with the window's bottom right corner.
            const sf::Vector2f offset = view_shft - item_shft;

            //Now move the text over.
            t.move(offset);

            return;
        };

/*
        textA.setString("Top Left\nTop Left");
        shift(textA, -1.0,  1.0);
        window.draw(textA);

        textA.setString("Bottom Left\nBottom Left");
        shift(textA, -1.0, -1.0);
        window.draw(textA);

        textA.setString("Top Right\nTop Right");
        shift(textA,  1.0,  1.0);
        window.draw(textA);

        textA.setString("Bottom Right\nBottom Right");
        shift(textA,  1.0, -1.0);
        window.draw(textA);

        textA.setString("Middle Middle Middle\nMiddle Middle Middle");
        shift(textA,  0.0,  0.0);
        window.draw(textA);
*/

/*
        shift(text_arr.at(0),  -0.75,  0.80);
        shift(text_arr.at(1),  -0.75,  0.00);
        shift(text_arr.at(2),  -0.75, -0.80);

        shift(text_arr.at(3),   0.75,  0.80);
        shift(text_arr.at(4),   0.75,  0.00);
        shift(text_arr.at(5),   0.75, -0.80);
*/        

        shift(text_arr.at(0),  -0.85,  0.50);
        shift(text_arr.at(1),  -0.85, -0.50);

        shift(text_arr.at(2),   0.00,  0.50);
        shift(text_arr.at(3),   0.00, -0.50);

        shift(text_arr.at(4),   0.85,  0.50);
        shift(text_arr.at(5),   0.85, -0.50);

        for(auto &t : text_arr){
            window.draw(t);
        }

        /*
        //Move the text to the bottom left corner.
        {
            const auto item_bbox = BLcornertext.getGlobalBounds();
            const auto item_blc  = sf::Vector2f( item_bbox.left, item_bbox.top + item_bbox.height );

            const auto wndw_view = window.getView();
            const auto view_cntr = wndw_view.getCenter();
            const auto view_size = wndw_view.getSize();
            const auto view_blc  = sf::Vector2f( view_cntr.x - 0.48f*view_size.x, view_cntr.y + 0.48f*view_size.y );

            //We should have that the item's bottom right corner coincides with the window's bottom right corner.
            const sf::Vector2f offset = view_blc - item_blc;

            //Now move the text over.
            BLcornertext.move(offset);
        }
        */

        /*
        //Move the text to the top right corner.
        {
            const auto item_bbox = contourtext.getGlobalBounds();
            const auto item_trc  = sf::Vector2f( item_bbox.left + item_bbox.width, item_bbox.top );

            const auto wndw_view = window.getView();
            const auto view_cntr = wndw_view.getCenter();
            const auto view_size = wndw_view.getSize();
            const auto view_trc  = sf::Vector2f( view_cntr.x + 0.48f*view_size.x, view_cntr.y - 0.48f*view_size.y );

            //We should have that the item's bottom right corner coincides with the window's bottom right corner.
            const sf::Vector2f offset = view_trc - item_trc;

            //Now move the text over.
            contourtext.move(offset);
        }
        */

        window.display();
    }

    return 0;
}
