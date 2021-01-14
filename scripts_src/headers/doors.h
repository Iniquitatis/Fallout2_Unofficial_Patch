#ifndef DOORS_H
#define DOORS_H

//Doors functions

#include "scripts.h"
#include "define.h"
#ifndef NAME
   #define NAME                    SCRIPT_DOOR
   #include "../headers/command.h"
#endif

/* Defines and Macros */

/* Door States */
#define STATE_ACTIVE                    (0)
#define STATE_INACTIVE                  (1)
#define STATE_WOOD                      (0)
#define STATE_METAL                     (1)
#define STATE_NON_DESTROY               (2)
#define STATE_STANDARD_LOCK             (0)
#define STATE_ELECTRIC_LOCK             (1)
#define STATE_DOOR_CLOSE                (1)
#define STATE_DOOR_NOCLOSE              (0)

#define door_mstr(x) (message_str(SCRIPT_DOOR,x))

/* Penalties for Lock difficulty based on whether or not you are using lockpicks. */
#ifndef Lock_Bonus
   #define Lock_Bonus                      (0)
#endif
#ifndef Lockpick_Bonus
   #define Lockpick_Bonus                  (Lock_Bonus+20)
#endif
#ifndef Exd_Lockpick_Bonus
   #define Exd_Lockpick_Bonus              (Lock_Bonus+40)
#endif

/* Penalties for disarming the trap */
#ifndef Trap_Bonus
   #define Trap_Bonus                      (0)
#endif
#ifndef Trap_Set_Bonus
   #define Trap_Set_Bonus                  (Trap_Bonus-10)
#endif
/* Experience Points for Skills */
#ifndef Lockpick_Exp
   #define Lockpick_Exp                    EXP_LOCKPICK_NORMAL
#endif
#ifndef Traps_Exp
   #define Traps_Exp                       EXP_TRAPS_NORMAL
#endif

/* Door close distance */
#ifndef DOOR_CLOSE_DIST
   #define DOOR_CLOSE_DIST                 (2)
/* How far do you want the last object that used the door to get away before it closes */
#endif

/* Timer id's */
#ifndef TIMER_CLOSE
   #define TIMER_CLOSE                     (1)
#endif

/* Penalties for forcing the door open using strength */
#ifndef Crowbar_Bonus
   #define Crowbar_Bonus                   (0)
#endif
#ifndef Crowbar_Strain
   #define Crowbar_Strain                  (2)
#endif

/* How many blasts can the door take before destorying */
#ifndef DOOR_STRENGTH
   #define DOOR_STRENGTH                   (2)
#endif

/* Max and Min damage for the trap */
#ifndef MIN_DAMAGE
   #define MIN_DAMAGE                      (10)
#endif
#ifndef MAX_DAMAGE
   #define MAX_DAMAGE                      (20)
#endif


/* Standard Script Procedures */
procedure start;
procedure use_p_proc;
procedure look_at_p_proc;
procedure description_p_proc;
procedure use_skill_on_p_proc;
procedure use_obj_on_p_proc;
procedure damage_p_proc;
procedure map_enter_p_proc;
procedure map_update_p_proc;

/* Script Specific Procedure Calls */
procedure Look_Traps_And_Locks;
procedure Look_Traps;
procedure Look_Locks;
procedure Skill_Disarm_Traps;
procedure Disarm_Traps;
procedure Unlock_Lock;
procedure Lockpick_Lock;
procedure Lockpick_Door;
procedure Lock_Door;
procedure Set_Lock;
procedure Set_Lockpick_Lock;
procedure Set_Trap;
procedure Pry_Door;
procedure Damage_Critter;
procedure Super_Lockpick_Lock;
procedure Super_Set_Lockpick_Lock;



/*****************************************************************
   Local Variables which are saved. All Local Variables need to be
   prepended by LVAR_
*****************************************************************/

#define LVAR_Locked                     (0)
#define LVAR_Trapped                    (1)
#define LVAR_Found_Trap                 (2)
#define LVAR_Set_Door_Status            (3)
#define LVAR_Explosion_Attempts         (4)
#define LVAR_Gave_Locks_XP              (5)
#define LVAR_Gave_Traps_XP              (6)


procedure trap_search_result(variable found_trap, variable who);
procedure trap_search_result(variable found_trap, variable who) begin
   if (found_trap == 0) then begin // can't see trap
      if (who == dude_obj) then begin
         display_msg(door_mstr(195));
      end else begin
         display_msg(obj_name(who) + door_mstr(200));
      end
   end else begin // found trap
      if (who == dude_obj) then begin
         display_msg(door_mstr(198));
      end else begin
         display_msg(obj_name(who) + door_mstr(200));
      end
   end
end

#define std_lockpick_inner_block \
   if (Tool == PID_LOCKPICKS) then begin \
      script_overrides; \
      if (local_var(LVAR_Locked) == STATE_ACTIVE) then begin \
         call Lockpick_Lock; \
      end else begin \
         call Set_Lockpick_Lock; \
      end \
   end \
   else if (Tool == PID_EXP_LOCKPICK_SET) then begin \
      script_overrides; \
      if (local_var(LVAR_Locked) == STATE_ACTIVE) then begin \
         call Super_Lockpick_Lock; \
      end else begin \
         call Super_Set_Lockpick_Lock; \
      end \
   end

#define std_lockpick_outer_block \
   if (LOCK_STATUS == STATE_STANDARD_LOCK) then begin \
      std_lockpick_inner_block \
   end

#define elec_lockpick_inner_block \
   if (Tool == PID_ELECTRONIC_LOCKPICKS) then begin \
      script_overrides; \
      if (local_var(LVAR_Locked) == STATE_ACTIVE) then begin \
         call Lockpick_Lock; \
      end else begin \
         call Set_Lockpick_Lock; \
      end \
   end \
   else if (Tool == PID_ELEC_LOCKPICK_MKII) then begin \
      script_overrides; \
      if (local_var(LVAR_Locked) == STATE_ACTIVE) then begin \
         call Super_Lockpick_Lock; \
      end else begin \
         call Super_Set_Lockpick_Lock; \
      end \
   end \

#define elec_lockpick_outer_block \
   else if (LOCK_STATUS == STATE_ELECTRIC_LOCK) then begin \
      elec_lockpick_inner_block \
   end


#define full_lockpick_block \
   std_lockpick_outer_block \
   elec_lockpick_outer_block



/**************************************************************************************
   Should the trap go off for any reason by critter influence, then this procedure will
   be called to deal damage to the critter, display a message stating how much damage
   was taken, and remove the trap.
**************************************************************************************/
#ifndef custom_Damage_Critter
procedure Damage_Critter begin
   variable Trap_Damage;

   Trap_Damage:=random(MIN_DAMAGE,MAX_DAMAGE);

   if (source_obj == dude_obj) then begin
       critter_dmg(dude_obj,Trap_Damage,(DMG_explosion BWOR DMG_BYPASS_ARMOR));
/* display_msg("You set off the trap and were hit for "+Trap_Damage+" points of damage.);                 */
       display_msg(door_mstr(166)+Trap_Damage+door_mstr(167));
   end

   else begin
       critter_dmg(source_obj,Trap_Damage,(DMG_explosion BWOR DMG_BYPASS_ARMOR));
/* display_msg(Critter_Name+" set off the trap was hit for "+Trap_Damage+" points of damage");        */
       display_msg(obj_name(source_obj)+door_mstr(168)+Trap_Damage+door_mstr(169));
   end

/* The trap is now disarmed and should never go off again.                           */
   set_local_var(LVAR_Trapped, STATE_INACTIVE);
end
#endif

/***************************************************************************
   This procedure is used should the player try to pry the door open using a
   crowbar or some similar instrument.
***************************************************************************/
#ifndef custom_Pry_Door
procedure Pry_Door begin
   variable Stat_Roll;

   Stat_Roll:=do_check(source_obj,STAT_st,Crowbar_Bonus);

   if (is_success(Stat_Roll)) then begin
       set_local_var(LVAR_Locked, STATE_INACTIVE);
       obj_unlock(self_obj);

       if (source_obj == dude_obj) then begin
           display_msg(door_mstr(176));
       end

       else begin
           display_msg(door_mstr(181));
       end
   end

   else if (is_critical(Stat_Roll)) then begin
       critter_dmg(source_obj,Crowbar_Strain,(DMG_normal_dam BWOR DMG_BYPASS_ARMOR));

       if (source_obj == dude_obj) then begin
           if (Crowbar_Strain == 1) then begin
               display_msg(door_mstr(177));
           end
           else begin
               display_msg(door_mstr(178)+Crowbar_Strain+door_mstr(179));
           end
       end

       else begin
           if (is_male(source_obj)) then begin
               if (Crowbar_Strain == 1) then begin
                   display_msg(door_mstr(182));
               end
               else begin
                   display_msg(door_mstr(183)+Crowbar_Strain+door_mstr(184));
               end
           end

           else begin
               if (Crowbar_Strain == 1) then begin
                   display_msg(door_mstr(186));
               end
               else begin
                   display_msg(door_mstr(187)+Crowbar_Strain+door_mstr(188));
               end
           end
       end
   end

   else begin
       if (source_obj == dude_obj) then begin
           display_msg(door_mstr(180));
       end

       else begin
           display_msg(door_mstr(185));
       end
   end
end
#endif

/**********************************************************************************
   This is called when the player is using an object on the door. When the check is
   made to find out what is being used, obj_pid(obj_being_used_with) will need to
   be checked against a prototype.
**********************************************************************************/
#ifndef custom_use_obj_on_p_proc
procedure use_obj_on_p_proc begin
   variable Tool;

   Tool:=obj_pid(obj_being_used_with);

   full_lockpick_block

   else if (Tool == PID_CROWBAR) then begin
       script_overrides; //added by killap
       call Pry_Door;
   end

   else if ((Tool == PID_DYNAMITE) or (Tool == PID_PLASTIC_EXPLOSIVES)) then begin
       script_overrides; //added by killap
       call Set_Trap;
   end

end
#endif


#endif // DOORS_H
